#ifndef HITTOP_CONCURRENT_ORDERED_ACTION_PAIR_H
#define HITTOP_CONCURRENT_ORDERED_ACTION_PAIR_H

#include <atomic>
#include <functional>
#include <memory>

#include "boost/smart_ptr/intrusive_ref_counter.hpp"

namespace hittop {
namespace concurrent {

class OrderedActionPair
    : public boost::intrusive_ref_counter<OrderedActionPair> {
public:
  enum PossibleStates {
    kInitial = 0,
    kRunFirstCallsSecond = 1,
    kRunSecondCallsSecond = 2
  };

  OrderedActionPair() { state_ = kInitial; }

  template <typename F> void RunFirst(F &&f) {
    std::forward<F>(f)();
    char current = state_.load(std::memory_order_acq_rel);
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

  template <typename G> void RunSecond(G &&g) {
    char current = state_.load(std::memory_order_acq_rel);
    if (current == kRunSecondCallsSecond) {
      std::forward<G>(g)();
      return;
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

private:
  std::atomic<char> state_;
  std::function<void()> second_;
};

} // namespace concurrent
} // namespace hittop

#endif // HITTOP_CONCURRENT_ORDERED_ACTION_PAIR_H
