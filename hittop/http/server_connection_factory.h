#ifndef HITTOP_HTTP_SERVER_CONNECTION_FACTORY_H
#define HITTOP_HTTP_SERVER_CONNECTION_FACTORY_H

namespace hittop {
namespace http {

// TODO - memory management policy
template <typename HandlerFactoryArgs, //
          typename HandlerFactory,     //
          typename SocketArgs,         //
          typename Socket>
class HttpServerConnectionFactory {
public:
  HttpServerConnectionFactory(boost::asio::io_service &io,
                              const HandlerFactoryArgs &factory_args,
                              const SocketArgs &socket_args)
      : io_(io), factory_args_(factory_args), socket_args_(socket_args) {}

  auto operator()() const {
    auto *const connection_ptr =
        new HttpServerConnection<HandlerFactory, Socket>(io_, factory_args_,
                                                         socket_args_);
    return std::make_tuple(connection_ptr,
                           [connection_ptr](const io::error_code &ec) {
                             std::cout << "connection finished" << std::endl;
                             delete connection_ptr;
                           });
  }

private:
  boost::asio::io_service &io_;
  HandlerFactoryArgs factory_args_;
  SocketArgs socket_args_;
};

} // namespace http
} // namespace hittop

#endif // HITTOP_HTTP_SERVER_CONNECTION_FACTORY_H
