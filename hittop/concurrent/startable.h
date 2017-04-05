#ifndef HITTOP_CONCURRENT_STARTABLE_H
#define HITTOP_CONCURRENT_STARTABLE_H

#include <assert.h>

#include <atomic>
#include <functional>

#include "hittop/util/swap_and_invoke.h"

#include "hittop/concurrent/error_code.h"

namespace hittop {
namespace concurrent {

// Abstract base class for AsyncTask types that notify when they have reached a
// well-defined "started" state and optionally provide a way to stop the task
// before its natural termination occurs.
//
// Derived types should provide a way to pass a Startable::StartHandler at
// construction time.  This handler type receives an error_code indicating
// whether the start was successful, and if it was, a nullary action function
// that can be invoked to ask the task the stop.  The StopAction need not
// provide any guarantees about whether the task will stop if it is invoked, or
// how soon.  Derived classes, if they offer stronger guarantees about this,
// should document their behavior accordingly.
//
class Startable : public AsyncTask<Startable> {
public:
  using StopAction = std::function<void()>;
  using StartHandler =
      std::function<void(const error_code &, const StopAction &)>;

  void AsyncRun(CompletionHandler completion_handler) final {
    completion_handler_ = std::move(completion_handler);
    started_.store(true, std::memory_order_release);

    Start([this](const error_code &ec) {
      CheckStarted();
      if (ec) {
        SwapAndInvoke(completion_handler_, ec);
      } else {
        SwapAndInvoke(start_handler_, ec, [this]() { Stop(); });
      }
    });
  }

protected:
  explicit Startable(StartHandler start_handler)
      : start_handler_(std::move(start_handler)) {
    started_.store(false, std::memory_order_relaxed);
  }

  // This function must be implemented to specify the startup logic for this
  // type; the passed completion handler must be invoked when the object is
  // started (not when it is finished; see Finished below).
  virtual void Start(const CompletionHandler &started) = 0;

  // Derived classes may override this method to provide a way to stop or cancel
  // a running task.
  virtual void Stop() {}

  // Derived class must call Finished as its final act; one should assume that
  // `delete this` is a side effect of calling this function and not touch any
  // object state after calling Finished().
  //
  // Finished does not need to be called in the case where the completion
  // handler passed to Start is invoked with a non-success error code,
  // indicating failure to start; behavior is undefined if Finished is called in
  // this case.
  void Finished(const error_code &ec) final {
    CheckStarted();
    SwapAndInvoke(completion_handler_, ec);
  }

private:
  // Asserts that AsyncRun has been invoked before now; also synchronizes all
  // writes made on the thread that called AsyncRun so they are visible on the
  // current thread.
  void CheckStarted() {
    const bool is_started = started_.load(std::memory_order_acquire);
    assert(is_started);
  }

  StartHandler start_handler_;
  CompletionHandler completion_handler_;
  std::atomic<bool> started_;
}:

} // namespace concurrent
} // namespace hittop

#endif // HITTOP_CONCURRENT_STARTABLE_H
