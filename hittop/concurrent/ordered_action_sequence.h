#ifndef HITTOP_CONCURRENT_ORDERED_ACTION_SEQUENCE_H
#define HITTOP_CONCURRENT_ORDERED_ACTION_SEQUENCE_H

#include "hittop/concurrent/ordered_action_pair.h"

#include "boost/intrusive_ptr.hpp"

namespace hittop {
namespace concurrent {

class OrderedActionSequence {
public:
  OrderedActionSequence() {
    // Burn the first action in the sequence by invoking RunFirst with a no-op.
    current_pair_->RunFirst([]() {});
  }

  // Returns a wrapper function that takes the same arguments as f.  The wrapper
  // will invoke f after all previous actions in this sequence have run.
  //
  template <typename F> auto WrapNext(F &&f) {
    using util::TailCall;

    boost::intrusive_ptr<OrderedActionPair> next_pair{new OrderedActionPair};
    boost::intrusive_ptr<OrderedActionPair> prev_pair = next_pair;
    current_pair_.swap(prev_pair);
    //
    // At this point, prev_pair is the old value of this->current_pair_, and
    // both this->current_pair_ and next_pair are the newly created
    // OrderedActionPair object.

    return [
      next_pair = std::move(next_pair), prev_pair = std::move(prev_pair),
      wrapped = std::forward<F>(f)
    ](auto &&... args) {
      TailCall tc = prev_pair->RunSecondTC([
        action = [ wrapped = std::move(wrapped), args... ]() {
          return wrapped(args...);
        },
        next_pair = std::move(next_pair)
      ]() { return next_pair->RunFirstTC(std::move(action)); });
      while (tc) {
        tc = tc();
      }
    };
  }

private:
  boost::intrusive_ptr<OrderedActionPair> current_pair_{new OrderedActionPair};
};

} // namespace concurrent
} // namespace hittop

#endif // HITTOP_CONCURRENT_ORDERED_ACTION_SEQUENCE_H
