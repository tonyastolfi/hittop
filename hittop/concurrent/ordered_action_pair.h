#ifndef HITTOP_CONCURRENT_ORDERED_ACTION_PAIR_H
#define HITTOP_CONCURRENT_ORDERED_ACTION_PAIR_H

#include <atomic>
#include <functional>
#include <memory>

#include "boost/smart_ptr/intrusive_ref_counter.hpp"

#include "hittop/util/tail_call.h"

namespace hittop {
namespace concurrent {

// Enforces ordering on two actions.  An action is considered any callable type
// that takes no arguments (return value is ignored unless it is of type
// TailCall; see RunFirstTC/RunSecondTC below).
//
class OrderedActionPair
    : public boost::intrusive_ref_counter<OrderedActionPair> {
public:
  using TailCall = util::TailCall;

  enum PossibleStates {
    kInitial = 0,
    kRunFirstCallsSecond = 1,
    kRunSecondCallsSecond = 2
  };

  OrderedActionPair() { state_ = kInitial; }

  // Returns the current state of the pair; should only be called after both
  // RunFirst and RunSecond have returned, in order to test which case was
  // exercised.
  //
  PossibleStates state() const {
    return static_cast<PossibleStates>(state_.load());
  }

  // Call f() and either call the action passed to RunSecond or arrange for
  // RunSecond to call its own action when it is called.
  //
  template <typename F> void RunFirst(F &&f) {
    std::forward<F>(f)();
    char current = state_.load(std::memory_order_acquire);
    for (;;) {
      if (current == kRunFirstCallsSecond) {
        second_();
        return;
      } else {
        if (state_.compare_exchange_weak(current, kRunSecondCallsSecond,
                                         std::memory_order_acq_rel)) {
          return;
        }
      }
    }
  }

  // Call f() and either arrange for RunSecond/RunSecondTC to run the second
  // action 'g' or return a non-empty TailCall that runs g and returns any
  // tail-position continuations of g.  Example usage:
  //
  // TailCall tc = pair->RunFirstTC(my_action);
  // while (tc) {
  //   tc = tc();
  // }
  //
  // Use this variant of RunSecond in cases where there may be a very large
  // number of nested tail calls within the passed action.  It allows a kind of
  // "manual" tail-call optimization that prevents the stack from growing.
  //
  template <typename F> TailCall RunFirstTC(F &&f) {
    std::forward<F>(f)();
    char current = state_.load(std::memory_order_acquire);
    for (;;) {
      if (current == kRunFirstCallsSecond) {
        // TODO - pull this out into a reusable SwapAndReturn function in util.
        TailCall k;
        k.swap(second_);
        return k;
      } else {
        if (state_.compare_exchange_weak(current, kRunSecondCallsSecond,
                                         std::memory_order_acq_rel)) {
          return {};
        }
      }
    }
  }

  // Either call g() or arrange for RunFirst to call a copy of g after it has
  // run the action passed to it.  If g is called by RunSecond, no copy will be
  // made.
  //
  template <typename G> void RunSecond(G &&g) {
    char current = state_.load(std::memory_order_acquire);
    if (current == kRunSecondCallsSecond) {
      return std::forward<G>(g)();
    }

    second_ = std::forward<G>(g);
    for (;;) {
      if (state_.compare_exchange_weak(current, kRunFirstCallsSecond,
                                       std::memory_order_acq_rel)) {
        return;
      }
      if (current == kRunSecondCallsSecond) {
        second_();
        return;
      }
    }
  }

  // Either arrange for RunFirst/RunFirstTC to run g (after the first action in
  // the ordered pair is performed) or return a non-empty TailCall that runs g
  // and returns any tail-position continuations of g.  Example usage:
  //
  // TailCall tc = pair->RunSecondTC(my_action);
  // while (tc) {
  //   tc = tc();
  // }
  //
  // Use this variant of RunSecond in cases where there may be a very large
  // number of nested tail calls within the passed action.  It allows a kind of
  // "manual" tail-call optimization that prevents the stack from growing.
  //
  template <typename G> TailCall RunSecondTC(G &&g) {
    char current = state_.load(std::memory_order_acquire);
    if (current == kRunSecondCallsSecond) {
      return g;
    }

    second_ = std::forward<G>(g);
    for (;;) {
      if (state_.compare_exchange_weak(current, kRunFirstCallsSecond,
                                       std::memory_order_acq_rel)) {
        return {};
      }
      if (current == kRunSecondCallsSecond) {
        TailCall k;
        k.swap(second_);
        return k;
      }
    }
  }

private:
  std::atomic<char> state_;
  TailCall second_;
};

} // namespace concurrent
} // namespace hittop

#endif // HITTOP_CONCURRENT_ORDERED_ACTION_PAIR_H
