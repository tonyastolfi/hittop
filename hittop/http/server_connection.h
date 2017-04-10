#ifndef HITTOP_HTTP_SERVER_CONNECTION_H
#define HITTOP_HTTP_SERVER_CONNECTION_H

#include "hittop/concurrent/async_parent_task.h"
#include "hittop/io/async_parent_task.h"
#include "hittop/util/construct_from_tuple.h"

namespace hittop {
namespace http {

template <typename HandlerFactory, typename Socket>
class ServerConnection
    : concurrent::AsyncParentTask<ServerConnection<HandlerFactory>> {
  friend class concurrent::AsyncParentTask<ServerConnection>;

public:
  using super_type = concurrent::AsyncParentTask<ServerConnection>;

  template <typename HandlerFactoryArgs, typename SocketArgs>
  explicit ServerConnection(boost::asio::io_service &io,
                            HandlerFactoryArgs &&handler_factory_args,
                            SocketArgs &&socket_args)
      : super_type(io), handler_factory_(std::forward<HandlerFactoryArgs>(
                            handler_factory_args)),
        socket_(std::forward<SocketArgs>(socket_args)) {}

private:
  void OnRun() {}

  ConstructFromTuple<HandlerFactory> handler_factory_;
  ConstructFromTuple<Socket> socket_;
  AsyncCircularBufferStream input_buffer_;
  AsyncReader reader_{*socket_, *input_buffer_};
};

} // namespace http
} // namespace hittop

#endif // HITTOP_HTTP_SERVER_CONNECTION_H
