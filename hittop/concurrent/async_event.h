#ifndef HITTOP_CONCURRENT_ASYNC_EVENT_H
#define HITTOP_CONCURRENT_ASYNC_EVENT_H

#include <atomic>
#include <cstdint>
#include <functional>
#include <memory>

#include "boost/smart_ptr/intrusive_ref_counter.hpp"

#include "hittop/util/scope_exit.h"
#include "hittop/util/swap_and_invoke.h"
#include "hittop/util/tail_call.h"

namespace hittop {
namespace concurrent {

// Enforces ordering on two actions.  An action is considered any callable type
// that takes no arguments (return value is ignored unless it is of type
// TailCall; see ReachedTC/ThenTC below).
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

  // Call f() and either call the action passed to Then or arrange for
  // Then to call its own action when it is called.
  //
  void Reached() {
    auto observed_state = LoadState(std::memory_order_acquire);
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

  // Call f() and either arrange for Then/ThenTC to run the second
  // action 'g' or return a non-empty TailCall that runs g and returns any
  // tail-position continuations of g.  Example usage:
  //
  // unwind(pair->ReachedTC();
  //
  // Use this variant of Then in cases where there may be a very large
  // number of nested tail calls within the passed action.  It allows a kind of
  // "manual" tail-call optimization that prevents the stack from growing.
  //
  TailCall ReachedTC() {
    auto observed_state = LoadState(std::memory_order_acquire);
    for (;;) {
      if (observed_state == kReachedCallsListener) {
        auto local_listener = std::move(listener_);
        util::SwapAndInvoke(cleanup_);
        return std::move(local_listener);
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
    auto observed_state = LoadState(std::memory_order_acquire);
    if (observed_state == kThenCallsListener) {
      return std::forward<Fn>(fn)();
    }
    assert(observed_state == kInitial);

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
    auto observed_state = LoadState(std::memory_order_acquire);
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
        return std::move(local_listener);
      }
    }
  }

private:
  PossibleStates LoadState(const std::memory_order order) const {
    return static_cast<PossibleStates>(state_.load(order));
  }

  Cleanup cleanup_;
  std::atomic<std::uint8_t> state_;
  TailCall listener_;
};

} // namespace concurrent
} // namespace hittop

#endif // HITTOP_CONCURRENT_ASYNC_EVENT_H
