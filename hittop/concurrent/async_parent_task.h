#ifndef HITTOP_CONCURRENT_ASYNC_PARENT_TASK_H
#define HITTOP_CONCURRENT_ASYNC_PARENT_TASK_H

#include <atomic>

#include "boost/asio/strand.hpp"

#include "hittop/concurrent/async_task.h"
#include "hittop/util/swap_and_invoke.h"

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
      // Decrement the child count atomically and if it is 1, then perform an
      // acquire so that this thread sees _all_ effects ordered before any other
      // decrements; this is to avoid using acq_rel ordering here.
      if (that_.child_count_.fetch_sub(1, std::memory_order_release) == 1) {
        std::atomic_thread_fence(std::memory_order_acquire);
        util::SwapAndInvoke(that_.handler_, error_code{});
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
    strand_.dispatch([ captured_handler = std::move(handler_arg), this ]() {
      CompletionCheckGuard g(derived_this());
      handler_ = std::move(captured_handler);
      derived_this().OnRun();
    });
  }

protected:
  explicit AsyncParentTask(boost::asio::io_service &io) : strand_(io) {
    // This reference will be released by AsyncRun.
    child_count_.store(1, std::memory_order_release);
  }

  template <typename Task>
  void Spawn(Task &&child_task, CompletionHandler handler) {
    // Run the child task, consuming the passed value using std::forward.
    std::forward<Task>(child_task)
        .AsyncRun(WrapChildHandler(std::move(handler)));
  }

  template <typename Handler> auto WrapChildHandler(Handler &&handler) {
    // Bump the refcount because we're launching a new child.
    child_count_.fetch_add(1, std::memory_order_relaxed);

    // The handler passed to the child task will dispatch the passed
    // handler to the parent task's strand, wrapping it with a reference
    // decrement (because the child task has terminated) and a
    // CompletionCheckGuard in case this is the last running child and
    // the handler doesn't Spawn any more tasks.
    return strand_.wrap(
        [ captured_handler = std::forward<Handler>(handler),
          this ](const error_code &ec) {
          CompletionCheckGuard g(derived_this());
          captured_handler(ec);
        });
  }

  // Expose the strand so that the derived task can run operations (via child
  // tasks) on the strand to access shared state and also so that it can get
  // at
  // the io_service associated with this task.
  boost::asio::io_service::strand strand_;

private:
  std::atomic<std::size_t> child_count_;
  CompletionHandler handler_;
};

} // namespace concurrent
} // namespace hittop

#endif // HITTOP_CONCURRENT_ASYNC_PARENT_TASK_H
