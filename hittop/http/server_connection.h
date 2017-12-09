#ifndef HITTOP_HTTP_SERVER_CONNECTION_H
#define HITTOP_HTTP_SERVER_CONNECTION_H

#include <tuple>
#include <utility>

#include "hittop/concurrent/async_parent_task.h"
#include "hittop/io/async_circular_buffer_stream.h"
#include "hittop/io/async_reader.h"
#include "hittop/util/construct_from_tuple.h"

namespace hittop {
namespace http {

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
        socket_(std::forward<SocketArgs>(socket_args)) {}

  Socket &socket() { return *socket_; }

private:
  void OnRun() {}

  util::ConstructFromTuple<HandlerFactory> handler_factory_;
  util::ConstructFromTuple<Socket> socket_;
  io::AsyncCircularBufferStream input_buffer_;
  io::AsyncReader<Socket &, io::AsyncCircularBufferStream &> reader_{
      std::make_tuple(std::ref(*socket_)),
      std::make_tuple(std::ref(input_buffer_))};
};

} // namespace http
} // namespace hittop

#endif // HITTOP_HTTP_SERVER_CONNECTION_H
