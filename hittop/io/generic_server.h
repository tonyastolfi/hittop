#ifndef HITTOP_IO_GENERIC_SERVER_H
#define HITTOP_IO_GENERIC_SERVER_H

#include <atomic>
#include <memory>

#include "hittop/concurrent/parent_startable.h"
#include "hittop/util/construct_from_tuple.h"

#include "hittop/io/types.h"

namespace hittop {
namespace io {

template <typename Derived, typename Acceptor, typename ConnectionFactory>
class GenericServer : public concurrent::ParentStartable<Derived> {
public:
  using super_type = concurrent::ParentStartable<Derived>;

  using StartHandler = typename super_type::StartHandler;
  using CompletionHandler = typename super_type::CompletionHandler;

  // ConnectionFactory is a callable type that returns a raw or smart pointer to
  // a Connection object.  Connection objects must be AsyncTasks with a `socket`
  // method (see below).
  using ConnectionPtr =
      std::decay_t<decltype(std::get<0>(std::declval<ConnectionFactory>()()))>;

  // Connection objects must expose a public method called `socket()` that
  // returns a reference to a socket object that is passed to
  // `Acceptor::async_accept`.
  using Socket =
      std::decay_t<decltype(std::declval<ConnectionPtr>()->socket())>;

  template <typename AcceptorArgs, typename ConnectionFactoryArgs>
  GenericServer(boost::asio::io_service &io, AcceptorArgs &&acceptor_args,
                ConnectionFactoryArgs &&connection_factory_args,
                StartHandler start_handler)
      : super_type(io, std::move(start_handler)),
        acceptor_(std::forward<AcceptorArgs>(acceptor_args)),
        connection_factory_(
            std::forward<ConnectionFactoryArgs>(connection_factory_args)) {
    stopped_ = false;
  }

protected:
  void Start(const CompletionHandler &started) override {
    AcceptNextConnection();
    started(error_code{});
  }

  void Stop() override {
    stopped_ = true;
    acceptor_->close();
  }

private:
  void AcceptNextConnection() {
    ConnectionPtr connection;
    CompletionHandler handler;
    std::tie(connection, handler) = (*connection_factory_)();
    auto &socket_ref = connection->socket();
    acceptor_->async_accept( //
        socket_ref, this->WrapChildHandler([
          captured_connection = std::move(connection),
          captured_handler = std::move(handler), this
        ](const auto &ec) {
          if (ec) {
            captured_handler(ec);
          } else {
            this->Spawn(*captured_connection, captured_handler);
          }
          if (stopped_) {
            this->Finished(ec);
          } else {
            AcceptNextConnection();
          }
        }));
  }

  util::ConstructFromTuple<Acceptor> acceptor_;
  util::ConstructFromTuple<ConnectionFactory> connection_factory_;
  std::atomic<bool> stopped_;
};

} // namespace io
} // namespace hittop

#endif // HITTOP_IO_GENERIC_SERVER_H
