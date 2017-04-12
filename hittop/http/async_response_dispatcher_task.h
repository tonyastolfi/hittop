#ifndef HITTOP_HTTP_ASYNC_RESPONSE_DISPATCHER_TASK_H
#define HITTOP_HTTP_ASYNC_RESPONSE_DISPATCHER_TASK_H

namespace hittop {
namespace http {

template <typename MutableBufferStream>
class AsyncResponseDispatcherTask
    : public AsyncTask<AsyncResponseDispatcherTask<MutableBufferStream>> {};

} // namespace http
} // namespace hittop

#endif // HITTOP_HTTP_ASYNC_RESPONSE_DISPATCHER_TASK_H
