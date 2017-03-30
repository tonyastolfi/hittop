#ifndef HITTOP_CONCURRENT_ORDERED_ACTION_PAIR_H
#define HITTOP_CONCURRENT_ORDERED_ACTION_PAIR_H

#include <atomic>
#include <functional>
#include <memory>

#include "boost/smart_ptr/intrusive_ref_counter.hpp"

namespace hittop {
namespace concurrent {

// Enforces ordering on two actions.  An action is considered any callable type
// that takes no arguments (return value is ignored).
//
class OrderedActionPair
    : public boost::intrusive_ref_counter<OrderedActionPair> {
public:
  enum PossibleStates {
    kInitial = 0,
    kRunFirstCallsSecond = 1,
    kRunSecondCallsSecond = 2
  };

  OrderedActionPair() { state_ = kInitial; }

  // Call f() and either call the action passed to RunSecond or arrange for
  // RunSecond to call its own action when it is called.
  //
  template <typename F> void RunFirst(F &&f) {
    std::forward<F>(f)();
    char current = state_.load(std::memory_order_acq_rel);
    for (;;) {
      if (current == kRunFirstCallsSecond) {
        return second_();
      } else {
        if (state_.compare_exchange_weak(current, kRunSecondCallsSecond,
                                         std::memory_order_acq_rel)) {
          return;
        }
      }
    }
  }

  // Either call g() or arrange for RunFirst to call a copy of g after it has
  // run the action passed to it.  If g is called by RunSecond, no copy will be
  // made.
  //
  template <typename G> void RunSecond(G &&g) {
    char current = state_.load(std::memory_order_acq_rel);
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
        return second_();
      }
    }
  }

private:
  std::atomic<char> state_;
  std::function<void()> second_;
};

} // namespace concurrent
} // namespace hittop

#endif // HITTOP_CONCURRENT_ORDERED_ACTION_PAIR_H
