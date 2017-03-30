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
    boost::intrusive_ptr<OrderedActionPair> next_pair{new OrderedActionPair};
    boost::intrusive_ptr<OrderedActionPair> prev_pair = next_pair;
    current_pair_.swap(prev_pair);
    return [
      next_pair = std::move(next_pair), prev_pair = std::move(prev_pair),
      wrapped = std::forward<F>(f)
    ](auto &&... args) {
      // TODO - this could very easily blow out the stack in its current form if
      // the number of out-of-order items in the sequence gets too large.
      // Implement some kind of stack trampoline to work around this issue (lack
      // of tail calls, doah!)
      //
      prev_pair->RunSecond([
        action =
            [ wrapped = std::move(wrapped), args... ]() { wrapped(args...); },
        next_pair = std::move(next_pair)
      ]() { next_pair->RunFirst(std::move(action)); });
    };
  }

private:
  boost::intrusive_ptr<OrderedActionPair> current_pair_{new OrderedActionPair};
};

} // namespace concurrent
} // namespace hittop

#endif // HITTOP_CONCURRENT_ORDERED_ACTION_SEQUENCE_H
