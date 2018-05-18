#ifndef HITTOP_CONCURRENT_ASYNC_EVENT_H
#define HITTOP_CONCURRENT_ASYNC_EVENT_H

#include <atomic>
#include <cstdint>
#include <functional>
#include <memory>

#include "boost/smart_ptr/intrusive_ref_counter.hpp"

#include "hittop/assert.h"
#include "hittop/util/scope_exit.h"
#include "hittop/util/swap_and_invoke.h"
#include "hittop/util/tail_call.h"

namespace hittop {
namespace concurrent {

// Provides a single-use synchronization point.  A cleanup function may be
// passed in at construction time; it will be invoked after the event has been
// Reached but before the Listener is invoked.
//
class AsyncEvent {
public:
  using Cleanup = std::function<void()>;
  using TailCall = util::TailCall;

  enum PossibleStates {
    kInitial = 0,
    kReachedCallsListener = 1,
    kThenCallsListener = 2
  };

  AsyncEvent() : cleanup_([]() {}) { state_ = kInitial; }

  AsyncEvent(const AsyncEvent &) = delete;
  AsyncEvent &operator=(const AsyncEvent &) = delete;

  template <typename F>
  explicit AsyncEvent(F &&cleanup_action)
      : cleanup_(std::forward<F>(cleanup_action)) {
    state_ = kInitial;
  }

  // Returns the current state of the pair; should only be called after both
  // Reached and Then have returned, in order to test which case was
  // exercised.
  //
  PossibleStates state() const { return LoadState(std::memory_order_seq_cst); }

  // Indicate that the event has happened; this allows the listener passed into
  // Then to be invoked.  The listener will be invoked directly from this method
  // if the call to Then happens before the call to Reached.  Calling this
  // method twice is illegal and will result in undefined behavior.
  //
  void Reached() {
    state_type observed_state = LoadState(std::memory_order_acquire);
    for (;;) {
      if (observed_state == kReachedCallsListener) {
        auto local_listener = std::move(listener_);
        util::SwapAndInvoke(cleanup_);
        local_listener();
        return;
      } else {
        // Acquire order is needed so that we can see all of `listener_`;
        // Release order is specified in addition as a convenience to
        // clients, allowing the AsyncEvent to act as a general memory fence.
        if (state_.compare_exchange_weak(observed_state, kThenCallsListener,
                                         std::memory_order_acq_rel)) {
          return;
        }
      }
    }
  }

  // Same as `Reached` except that the listener is never invoked from within
  // `ReachedTC`.  The TailCall should be resolved using:
  //
  // unwind(event.ReachedTC();
  //
  // Use this variant of `Reached` in cases where there may be a very large
  // number of nested tail calls within the listener.  It allows a kind of
  // "manual" tail-call optimization that prevents the stack from growing.
  //
  TailCall ReachedTC() {
    state_type observed_state = LoadState(std::memory_order_acquire);
    for (;;) {
      if (observed_state == kReachedCallsListener) {
        auto local_listener = std::move(listener_);
        util::SwapAndInvoke(cleanup_);
        return local_listener;
      } else {
        // See comment in Reached above.
        if (state_.compare_exchange_weak(observed_state, kThenCallsListener,
                                         std::memory_order_acq_rel)) {
          return {};
        }
      }
    }
  }

  // Either call g() or arrange for Reached to call a copy of g after it has
  // run the action passed to it.  If g is called by Then, no copy will be
  // made.
  //
  template <typename Fn> void Then(Fn &&fn) {
    state_type observed_state = LoadState(std::memory_order_acquire);
    if (observed_state == kThenCallsListener) {
      util::SwapAndInvoke(cleanup_);
      std::forward<Fn>(fn)();
      return;
    }
    HITTOP_ASSERT(observed_state == kInitial);

    listener_ = std::forward<Fn>(fn);
    for (;;) {
      if (state_.compare_exchange_weak(observed_state, kReachedCallsListener,
                                       std::memory_order_acq_rel)) {
        return;
      }
      if (observed_state == kThenCallsListener) {
        auto local_listener = std::move(listener_);
        util::SwapAndInvoke(cleanup_);
        local_listener();
        return;
      }
    }
  }

  // Either arrange for Reached/ReachedTC to run g (after the first action in
  // the ordered pair is performed) or return a non-empty TailCall that runs g
  // and returns any tail-position continuations of g.  Example usage:
  //
  // unwind(pair->ThenTC(my_action));
  //
  // Use this variant of Then in cases where there may be a very large
  // number of nested tail calls within the passed action.  It allows a kind of
  // "manual" tail-call optimization that prevents the stack from growing.
  //
  template <typename Fn> TailCall ThenTC(Fn &&fn) {
    state_type observed_state = LoadState(std::memory_order_acquire);
    if (observed_state == kThenCallsListener) {
      util::ScopeExit janitor(std::move(cleanup_));
      return std::forward<Fn>(fn);
    }

    listener_ = std::forward<Fn>(fn);
    for (;;) {
      if (state_.compare_exchange_weak(observed_state, kReachedCallsListener,
                                       std::memory_order_acq_rel)) {
        return {};
      }
      if (observed_state == kThenCallsListener) {
        auto local_listener = std::move(listener_);
        util::SwapAndInvoke(cleanup_);
        return local_listener;
      }
    }
  }

private:
  using state_type = std::uint8_t;

  PossibleStates LoadState(const std::memory_order order) const {
    return static_cast<PossibleStates>(state_.load(order));
  }

  Cleanup cleanup_;
  std::atomic<state_type> state_;
  TailCall listener_;
};

} // namespace concurrent
} // namespace hittop

#endif // HITTOP_CONCURRENT_ASYNC_EVENT_H
