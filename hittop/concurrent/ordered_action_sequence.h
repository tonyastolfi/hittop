#ifndef HITTOP_CONCURRENT_ORDERED_ACTION_SEQUENCE_H
#define HITTOP_CONCURRENT_ORDERED_ACTION_SEQUENCE_H

#include "hittop/concurrent/ordered_action_pair.h"

#include "boost/intrusive_ptr.hpp"

namespace hittop {
namespace concurrent {

class OrderedActionSequence {
public:
  OrderedActionSequence() {
    current_pair_->RunFirst([]() {});
  }

  template <typename F> auto WrapNext(F &&f) {
    boost::intrusive_ptr<OrderedActionPair> next_pair{new OrderedActionPair};
    boost::intrusive_ptr<OrderedActionPair> prev_pair = next_pair;
    current_pair_.swap(prev_pair);
    return [
      next_pair = std::move(next_pair), prev_pair = std::move(prev_pair),
      wrapped = std::forward<F>(f)
    ](auto &&... args) {
      prev_pair->RunSecond([
        action = [ wrapped = std::forward<F>(wrapped), args... ]() {
          std::forward<F>(wrapped)(args...);
        },
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
