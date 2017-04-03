#ifndef HITTOP_CONCURRENT_ASYNC_TASK_H
#define HITTOP_CONCURRENT_ASYNC_TASK_H

#include <functional>
#include <type_traits>

#include "hittop/concurrent/error_code.h"

namespace hittop {
namespace concurrent {

class AsyncTaskBase {
public:
  using CompletionHandler = std::function<void(const error_code &)>;
};

template <typename Derived, typename Base = AsyncTaskBase>
class AsyncTask : public Base {
public:
  using CompletionHandler = typename Base::CompletionHandler;

protected:
  AsyncTask() {
    static_assert(
        std::is_same<decltype(std::declval<Derived>().AsyncRun(
                         std::declval<AsyncTaskBase::CompletionHandler>())),
                     void>::value,
        "Models of concept AsyncTask must expose a public member "
        "function AsyncRun that takes an AsyncTask::CompletionHandler and "
        "returns void.");
  }
};

} // namespace concurrent
} // namespace hittop

#endif // HITTOP_CONCURRENT_ASYNC_TASK_H
