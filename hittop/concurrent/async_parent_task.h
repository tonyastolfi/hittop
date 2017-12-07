#ifndef HITTOP_CONCURRENT_ASYNC_PARENT_TASK_H
#define HITTOP_CONCURRENT_ASYNC_PARENT_TASK_H

#include <atomic>

#include "boost/asio/strand.hpp"

#include "hittop/concurrent/async_task.h"

namespace hittop {
namespace concurrent {

template <typename Derived, typename Base = AsyncTaskBase>
class AsyncParentTask : public AsyncTask<Derived, Base> {
private:
  // Releases a child count reference on destruction; if the count becomes zero,
  // invoke the tasks's completion handler.
  struct CompletionCheckGuard {
    CompletionCheckGuard(const CompletionCheckGuard &) = delete;
    CompletionCheckGuard &operator=(const CompletionCheckGuard &) = delete;

    explicit CompletionCheckGuard(Derived &that) : that_(that) {}

    ~CompletionCheckGuard() {
      if (that_.child_count_.fetch_sub(1, std::memory_order_release) == 0) {
        std::atomic_thread_fence(std::memory_order_acquire);
        decltype(that_.handler_) local_handler;
        local_handler.swap(that_.handler_);
        local_handler({});
      }
    }

    Derived &that_;
  };

  Derived &derived_this() & { //
    return static_cast<Derived &>(*this);
  }

public:
  using CompletionHandler = typename Base::CompletionHandler;

  AsyncParentTask(const AsyncParentTask &) = delete;
  AsyncParentTask &operator=(const AsyncParentTask &) = delete;

  void AsyncRun(CompletionHandler handler_arg) {
    // We must be sure to run this on the strand so that there aren't any races
    // between Derived::OnRun and the completion handlers of any Spawned child
    // tasks.
    strand_.dispatch([
      derived = derived_this(), captured_handler = std::move(handler_arg)
    ]() {
      CompletionCheckGuard g(*derived);
      handler_ = std::move(captured_handler);
      derived.OnRun();
    });
  }

protected:
  explicit AsyncParentTask(boost::asio::io_service &io) : strand_(io) {
    // This reference will be released by AsyncRun.
    child_count_.store(1, std::memory_order_release);
  }

  template <typename Task>
  void Spawn(Task &&child_task, CompletionHandler handler) {
    // Bump the refcount because we're launching a new child.
    child_count_.fetch_add(1, std::memory_order_relaxed);

    // Run the child task, consuming the passed value using std::forward.
    std::forward<Task>(child_task)
        .AsyncRun(
            // The handler passed to the child task will dispatch the passed
            // handler to the parent task's strand, wrapping it with a reference
            // decrement (because the child task has terminated) and a
            // CompletionCheckGuard in case this is the last running child and
            // the handler doesn't Spawn any more tasks.
            strand_.wrap(
                [ handler = std::move(handler), this ](const error_code &ec) {
                  CompletionCheckGuard g(derived_this());
                  handler(ec);
                }));
  }

  // Expose the strand so that the derived task can run operations (via child
  // tasks) on the strand to access shared state and also so that it can get at
  // the io_service associated with this task.
  boost::asio::io_service::strand strand_;

private:
  std::atomic<std::size_t> child_count_;
  CompletionHandler handler_;
};

} // namespace concurrent
} // namespace hittop

#endif // HITTOP_CONCURRENT_ASYNC_PARENT_TASK_H
