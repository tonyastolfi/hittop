#ifndef HITTOP_CONCURRENT_ASYNC_QUEUE_H
#define HITTOP_CONCURRENT_ASYNC_QUEUE_H

#include <deque>
#include <functional>
#include <mutex>
#include <type_traits>

#include "boost/asio/error.hpp"
#include "boost/optional.hpp"
#include "boost/smart_ptr/intrusive_ptr.hpp"
#include "boost/smart_ptr/intrusive_ref_counter.hpp"
#include "boost/system/error_code.hpp"
#include "boost/utility/in_place_factory.hpp"

#include "hittop/util/scope_exit.h"

namespace hittop {
namespace concurrent {

// Asynchronous queue.  Makes no guarantees about the relative orders in which
// consumers will see items; only that each consumer will be matched up with
// one item.  FIFO ordering is best-effort; consumers are run ASAP within
// push/pop methods.
//
// If you want to ensure FIFO, then do BOTH:
//   1. serialize all pushes according to the desired order
//   2. only allow one pending async_pop operation at a time
//
template <typename T, typename Mutex = std::mutex> class AsyncQueue {
public:
  using PopCanceler = std::function<bool()>;

  AsyncQueue() = default;
  AsyncQueue(const AsyncQueue &) = delete;
  AsyncQueue &operator=(const AsyncQueue &) = delete;

  void push(const T &item) { GenericInsert<Push>(item); }

  void emplace(T &&item) { GenericInsert<Emplace>(std::move(item)); }

  template <typename F> PopCanceler async_pop(F &&handler) {
    boost::optional<T> item;
    {
      std::unique_lock<Mutex> lock(mutex_);
      if (!inventory_.empty()) {
        item = std::move(inventory_.front());
        inventory_.pop_front();
      } else {
        boost::intrusive_ptr<Consumer> consumer(
            new ConsumerImpl<typename std::decay<F>::type>(
                std::forward<F>(handler)));
        consumers_.push_back(consumer);
        return [consumer = std::move(consumer)]() {
          return consumer->Consume(boost::asio::error::operation_aborted,
                                   nullptr);
        };
      }
    }
    handler(boost::system::error_code{}, &*item);
    return {};
  }

private:
  class Consumer : public boost::intrusive_ref_counter<Consumer> {
  public:
    virtual ~Consumer() {}

    virtual bool Consume(const boost::system::error_code &, T *) = 0;
  };

  template <typename F> class ConsumerImpl : public Consumer {
  public:
    template <typename A>
    explicit ConsumerImpl(A &&a) : f_(boost::in_place(std::forward<A>(a))) {
      active_ = true;
    }

    bool Consume(const boost::system::error_code &ec, T *item) override {
      if (!active_.exchange(false)) {
        return false;
      }
      util::ScopeExit clear_f([this]() { f_ = boost::none; });
      (*f_)(ec, item);
      return true;
    }

  private:
    // TODO(tonyastolfi) - replace this with "creative" use of the intrusive
    // count (which is also atomic/thread-safe) as an optimization
    std::atomic<bool> active_;
    boost::optional<F> f_;
  };

  struct Emplace {
    AsyncQueue *that_;
    T &&item_;

    Emplace(AsyncQueue *that, T &&item) : that_(that), item_(std::move(item)) {}

    bool DispatchTo(Consumer *const c) { return c->Consume({}, &item_); }

    void Insert() { that_->inventory_.emplace_back(std::move(item_)); }
  };

  struct Push {
    AsyncQueue *that_;
    const T *item_;
    boost::optional<T> copy_;

    Push(AsyncQueue *that, const T &item) : that_(that), item_(&item) {}

    bool DispatchTo(Consumer *const c) {
      if (!copy_) {
        copy_ = *item_;
      }
      return c->Consume({}, &*copy_);
    }

    void Insert() {
      if (copy_) {
        that_->inventory_.emplace_back(std::move(*copy_));
      } else {
        that_->inventory_.push_back(*item_);
      }
    }
  };

  template <typename Op, typename A> void GenericInsert(A &&a) {
    Op op(this, std::forward<A>(a));
    std::unique_lock<Mutex> lock(mutex_);
    while (!consumers_.empty()) {
      boost::intrusive_ptr<Consumer> consumer = std::move(consumers_.front());
      consumers_.pop_front();
      lock.unlock();
      if (op.DispatchTo(consumer.get())) {
        return;
      }
      lock.lock();
    }
    op.Insert();
  }

  Mutex mutex_;
  std::deque<T> inventory_;
  std::deque<boost::intrusive_ptr<Consumer>> consumers_;
};

} // namespace concurrent
} // namespace hittop

#endif // HITTOP_CONCURRENT_ASYNC_QUEUE_H
