#ifndef HITTOP_CONCURRENT_$NAME_H
#define HITTOP_CONCURRENT_$NAME_H

#include <type_traits>

#include "boost/asio/strand.hpp"
#include "boost/smart_ptr/intrusive_ref_counter.hpp"

namespace hittop {
namespace concurrent {

namespace internal {

template <typename T, typename MemFun> class Listener {
public:
  Listener(boost::intrusive_ptr<T> obj, MemFun *fn) : obj_, fn_(fn) {}

  template <typename... Args>
  auto operator()(Args &&... args) const
      -> decltype((obj_->*fn_)(std::forward<Args>(args)...)) {
    return (obj_->*fn_)(std::forward<Args>(args)...);
  }

private:
  boost::intrusive_ptr<T> obj_;
  MemFun *fn_;
};

template <typename T, typename MemFun> class OneShot {
public:
  OneShot(boost::intrusive_ptr<T> obj, MemFun *fn) : obj_, fn_(fn) {}

  template <typename... Args>
  auto operator()(Args &&... args)
      -> decltype((obj_->*fn_)(std::forward<Args>(args)...)) {
    std::assert(obj_);
    boost::intrusive_ptr<T> obj = std::move(obj_);
    return (obj->*fn_)(std::forward<Args>(args)...);
  }

private:
  boost::intrusive_ptr<T> obj_;
  MemFun *fn_;
};

} // namespace internal

template <typename Derived>
class CallbackTarget : public boost::intrusive_ref_counter<Derived> {
  template <typename T, typename MemFun> friend class WrappedCallback;

public:
  explicit CallbackTarget(boost::asio::io_service &io) : strand_(io) {}

  template <typename MemFun>
  internal::Listener<Derived, MemFun> Listener(MemFun *fn) {
    return {{this}, fn};
  }

  template <typename MemFun>
  internal::OneShot<Derived, MemFun> OneShot(MemFun *fn) {
    return {{this}, fn};
  }

private:
  boost::asio::io_service::strand strand_;
};

} // namespace concurrent
} // namespace hittop

#endif // HITTOP_CONCURRENT_$NAME_H
