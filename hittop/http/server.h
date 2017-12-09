#ifndef HITTOP_HTTP_SERVER_H
#define HITTOP_HTTP_SERVER_H

#include <memory>
#include <tuple>
#include <type_traits>
#include <utility>

#include "boost/asio/io_service.hpp"
#include "boost/asio/ip/tcp.hpp"
#include "boost/numeric/conversion/cast.hpp"

#include "hittop/io/generic_server.h"

#include "hittop/http/server_config.pb.h"
#include "hittop/http/server_connection.h"
#include "hittop/http/server_connection_factory.h"

namespace hittop {
namespace http {

template <typename HandlerFactory, typename HandlerFactoryArgs>
class HttpServer;

template <typename HandlerFactory, typename HandlerFactoryArgs>
using HttpServerBase = io::GenericServer<
    HttpServer<HandlerFactory, HandlerFactoryArgs>,
    boost::asio::ip::tcp::acceptor,
    HttpServerConnectionFactory<
        HandlerFactoryArgs, HandlerFactory,
        std::tuple<std::reference_wrapper<boost::asio::io_service>>,
        boost::asio::ip::tcp::socket>>;

template <typename HandlerFactory,
          typename HandlerFactoryArgs =
              std::tuple<std::reference_wrapper<const HandlerFactory>>>
class HttpServer : public HttpServerBase<HandlerFactory, HandlerFactoryArgs> {
public:
  using super_type = HttpServerBase<HandlerFactory, HandlerFactoryArgs>;

  using StartHandler = typename super_type::StartHandler;

  template <typename Args>
  HttpServer(boost::asio::io_service &io, HttpServerConfig config,
             Args &&handler_factory_args, StartHandler start_handler)
      : super_type(io,
                   // Acceptor args:
                   std::forward_as_tuple(
                       std::ref(io),
                       boost::asio::ip::tcp::endpoint{
                           boost::asio::ip::address_v4::any(),
                           boost::numeric_cast<unsigned short>(config.port())}),
                   // ConnectionFactory args:
                   std::forward_as_tuple(
                       std::ref(io), std::forward<Args>(handler_factory_args),
                       std::forward_as_tuple(std::ref(io))),
                   std::move(start_handler)),
        config_(std::move(config)) {}

private:
  const HttpServerConfig config_;
};

} // namespace http
} // namespace hittop

#endif // HITTOP_HTTP_SERVER_H
