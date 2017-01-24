#ifndef HITTOP_CONCURRENT_CALLBACK_TARGET_H
#define HITTOP_CONCURRENT_CALLBACK_TARGET_H

#include <assert.h>

#include <functional>
#include <type_traits>

#include "boost/asio/strand.hpp"
#include "boost/smart_ptr/intrusive_ptr.hpp"
#include "boost/smart_ptr/intrusive_ref_counter.hpp"

#include "hittop/util/tuples.h"

namespace hittop {
namespace concurrent {

struct PostScheduler {
  template <typename Task>
  static void schedule(boost::asio::io_service::strand &strand, Task &&task) {
    strand.post(std::forward<Task>(task));
  }
};

struct DispatchScheduler {
  template <typename Task>
  static void schedule(boost::asio::io_service::strand &strand, Task &&task) {
    strand.dispatch(std::forward<Task>(task));
  }
};

template <typename T, typename MemFun, typename Scheduler>
class PointerRetainedCallback {
public:
  PointerRetainedCallback(boost::intrusive_ptr<T> obj, MemFun fn)
      : obj_(std::move(obj)), fn_(fn) {}

  template <typename... Args> void operator()(Args &&... args) const {
    Scheduler::schedule(obj_->get_strand(), [
      obj = obj_, fn = fn_,
      arg_pack = std::make_tuple(obj_.get(), std::forward<Args>(args)...)
    ]() { return util::tuples::Apply(std::mem_fn(fn), std::move(arg_pack)); });
  }

private:
  boost::intrusive_ptr<T> obj_;
  MemFun fn_;
};

template <typename T, typename MemFun, typename Scheduler>
class PointerTransferredCallback {
public:
  PointerTransferredCallback(boost::intrusive_ptr<T> obj, MemFun fn)
      : obj_(std::move(obj)), fn_(fn) {}

  template <typename... Args> void operator()(Args &&... args) {
    assert(obj_);
    auto bound_obj = obj_;
    auto local_obj = std::move(obj_);
    Scheduler::schedule(local_obj->get_strand(), [
      obj = std::move(bound_obj), fn = fn_,
      arg_pack = std::make_tuple(local_obj.get(), std::forward<Args>(args)...)
    ]() { return util::tuples::Apply(std::mem_fn(fn), std::move(arg_pack)); });
  }

private:
  boost::intrusive_ptr<T> obj_;
  MemFun fn_;
};

template <typename Derived>
class CallbackTarget : public boost::intrusive_ref_counter<Derived> {
protected:
  explicit CallbackTarget(boost::asio::io_service &io) : strand_(io) {
    static_assert(std::is_base_of<CallbackTarget<Derived>, Derived>::value,
                  "CallbackTarget<T> must be passed a derived class as its "
                  "template argument.");
  }

public:
  CallbackTarget(const CallbackTarget &) = delete;
  CallbackTarget &operator=(const CallbackTarget &) = delete;

  boost::intrusive_ptr<Derived> shared_from_this() {
    return boost::intrusive_ptr<Derived>(static_cast<Derived *>(this));
  }

  boost::asio::io_service::strand &get_strand() { return strand_; }

  template <typename MemFun>
  PointerRetainedCallback<Derived, MemFun, DispatchScheduler>
  ListenerCallback(MemFun fn) {
    return {shared_from_this(), fn};
  }

  template <typename MemFun>
  PointerTransferredCallback<Derived, MemFun, DispatchScheduler>
  OneShotCallback(MemFun fn) {
    return {shared_from_this(), fn};
  }

  // Callback generators that schedule invocations using post, ensuring "later"
  // execution.

  template <typename MemFun>
  PointerRetainedCallback<Derived, MemFun, PostScheduler>
  ListenerCallbackLater(MemFun fn) {
    return {shared_from_this(), fn};
  }

  template <typename MemFun>
  PointerTransferredCallback<Derived, MemFun, PostScheduler>
  OneShotCallbackLater(MemFun fn) {
    return {shared_from_this(), fn};
  }

  // Programmatically select policies

  template <template <typename, typename, typename>
            class PointerRetentionPolicy,
            typename Scheduler, typename MemFun>
  PointerRetentionPolicy<Derived, MemFun, Scheduler> Callback(MemFun fn) {
    return {shared_from_this(), fn};
  }

private:
  boost::asio::io_service::strand strand_;
};

} // namespace concurrent
} // namespace hittop

#endif // HITTOP_CONCURRENT_CALLBACK_TARGET_H
