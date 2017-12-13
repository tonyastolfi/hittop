#ifndef HITTOP_HTTP_SERVER_CONNECTION_H
#define HITTOP_HTTP_SERVER_CONNECTION_H

#include <experimental/string_view>
#include <functional>
#include <tuple>
#include <utility>

#include "boost/asio/buffer.hpp"
#include "boost/asio/buffers_iterator.hpp"

#include "hittop/concurrent/async_parent_task.h"
#include "hittop/concurrent/ordered_action_pair.h"
#include "hittop/io/async_circular_buffer_stream.h"
#include "hittop/io/async_reader.h"
#include "hittop/kwargs/kwargs.h"
#include "hittop/util/construct_from_tuple.h"
#include "hittop/util/shared_ptr_type.h"

#include "hittop/http/connection_disposition.h"
#include "hittop/http/parse_request.h"
#include "hittop/http/request.h"

namespace hittop {
namespace http {

using string_view = std::experimental::string_view;

DEFINE_KEYWORD(status, unsigned);
DEFINE_KEYWORD(message, string_view);
DEFINE_KEYWORD(
    headers,
    std::function<void(std::function<void(string_view, string_view)>)>);
DEFINE_KEYWORD(connection, ConnectionDisposition);
DEFINE_KEYWORD(body, io::GenericConstBufferStreamBase *);
DEFINE_KEYWORD(cleanup, std::function<void(const io::error_code &)>);

template <typename HandlerFactory, typename Socket>
class HttpServerConnection : public concurrent::AsyncParentTask<
                                 HttpServerConnection<HandlerFactory, Socket>> {
  friend class concurrent::AsyncParentTask<HttpServerConnection>;

public:
  using super_type = concurrent::AsyncParentTask<HttpServerConnection>;

  template <typename HandlerFactoryArgs, typename SocketArgs>
  HttpServerConnection(boost::asio::io_service &io,
                       HandlerFactoryArgs &&handler_factory_args,
                       SocketArgs &&socket_args)
      : super_type(io), handler_factory_(std::forward<HandlerFactoryArgs>(
                            handler_factory_args)),
        socket_(std::forward<SocketArgs>(socket_args)), input_buffer_(),
        reader_(std::forward_as_tuple(std::ref(*socket_)),
                std::forward_as_tuple(std::ref(input_buffer_))) {}

  Socket &socket() { return *socket_; }

private:
  void OnRun() {
    std::cout << "started a new connection" << std::endl;
    this->Spawn(reader_, [](const io::error_code &ec) {
      std::cout << "done reading" << std::endl;
    });
    ReadNextRequest();
  }

  void ReadNextRequest(const std::size_t minimum_bytes = 1) {
    input_buffer_.async_fetch(minimum_bytes, this->WrapChildHandler([this](
                                                 const io::error_code &ec,
                                                 const auto &buffers) {
      if (ec) {
        std::cout << "error fetching from buffer" << std::endl;
        return;
      }
      auto first = boost::asio::buffers_begin(buffers);
      auto last = boost::asio::buffers_end(buffers);
      std::cout << "read data:" << std::string(first, last) << std::flush;

      ZeroCopyRequest<decltype(first)> request;
      auto result =
          ParseRequest(boost::make_iterator_range(first, last), &request);
      if (result.ok()) {
        std::cout << "Parsed OK!" << std::endl;
        last_request_size_ = std::distance(first, result.get());
        auto handler = (*handler_factory_)();
        handler( //
            request,
            // continue_with(error, body_reader) or continue_with(error)
            this->WrapChildHandler(
                [this](const io::error_code &ec, auto &&body_reader) {
                  std::cout << "handler said " << s << std::endl;
                  input_buffer_.consume(last_request_size_);
                  // TODO - give body to the handler.
                  ReadNextRequest();
                })
            // TODO - respond_with(status, message, disposition, headers, body)
            //  (using named parameters;
            //   respond_with(
            //     http::status = 200,
            //     http::message = "OK",
            //     http::headers = [](auto&& header) {
            //       header("Content-Type": "application/json");
            //       header("Connection": "close");
            //     },
            //     http::body = <pointer-like to AsyncConstBufferStream>
            //     http::cleanup = []() {
            //       delete buffer_stream; ...
            //     }
            //   )
            );
      } else if (result.error() == parser::ParseError::INCOMPLETE) {
        const std::size_t new_minimum = boost::asio::buffer_size(buffers) + 1;
        input_buffer_.consume(0);
        ReadNextRequest(new_minimum);
      } else {
        std::cout << "Parse bad" << std::endl;
        input_buffer_.consume(0);
        socket_->close();
      }
    }));
  }

  util::ConstructFromTuple<HandlerFactory> handler_factory_;
  util::ConstructFromTuple<Socket> socket_;
  io::AsyncCircularBufferStream input_buffer_;
  io::AsyncReader<Socket &, io::AsyncCircularBufferStream &> reader_;
  std::size_t last_request_size_ = 0;
  util::shared_ptr_t<concurrent::OrderedActionPair> next_response_boundary_{
      new concurrent::OrderedActionPair{}};
};

} // namespace http
} // namespace hittop

#endif // HITTOP_HTTP_SERVER_CONNECTION_H
