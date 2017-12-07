#ifndef HITTOP_HTTP_SERVER_H
#define HITTOP_HTTP_SERVER_H

#include <memory>
#include <type_traits>

#include "boost/asio/io_service.hpp"
#include "boost/asio/ip/tcp.hpp"
#include "boost/numeric/conversion/cast.hpp"

#include "hittop/concurrent/callback_target.h"
#include "hittop/concurrent/startable.h"

#include "hittop/http/server_config.pb.h"
#include "hittop/http/server_connection.h"

namespace hittop {
namespace http {

namespace internal {

class AllConnectionsTask : public AsyncParentTask<AllConnectionsTask> {
public:
  void OnRun() {}
};

} // namespace internal

// TODO - derive from concurrent::ParentStartable instead of these two
template <typename ConnectionFactory>
class HttpServer : public concurrent::Startable,
                   public concurrent::CallbackTarget<HttpServer> {
public:
  using self_type = HttpServer;

  template <typename ConnectionFactoryArgs>
  HttpServer(boost::asio::io_service &io, HttpServerConfig config,
             ConnectionFactoryArgs &&connection_factory_args,
             StartHandler start_handler)
      : concurrent::Startable(std::move(start_handler)),
        concurrent::CallbackTarget<HttpServer>(io), config_(config), io_(io),
        connection_factory(
            std::forward<ConnectionFactoryArgs>(connection_factory_args)) {}

protected:
  void Start(const CompletionHandler &started) override {
    OneShotCallback (&self_type::SafeStart)(started);
  }

  void Stop() override { acceptor_.close(); }

private:
  using ConnectionPtr =
      std::decay_t<decltype(std::declval<ConnectionFactory>()())>;

  void SafeStart(const CompletionHandler &started) { started({}); }

  void StartAccept() {
    auto connection = connection_factory_();
    acceptor_.async_accept(*socket, [
      connection = std::move(connection),
      callback = OneShotCallback(&self_type::HandleAccept)
    ](const auto &ec) { callback(ec, connection); });
  }

  void HandleAccept(const boost::system::error_code &ec,
                    const ConnectionPtr &connection) {
    connection->AsyncRun();
  }

  const HttpServerConfig config_;
  boost::asio::io_service &io_;
  boost::asio::ip::tcp::acceptor acceptor_{
      io_,
      {boost::asio::ip::address_v4::any(),
       boost::numeric_cast<unsigned short>(config_.port())}};
  ConstructFromTuple<ConnectionFactory> connection_factory_;
};

} // namespace http
} // namespace hittop

#endif // HITTOP_HTTP_SERVER_H
