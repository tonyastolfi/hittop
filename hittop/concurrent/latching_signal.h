#ifndef HITTOP_CONCURRENT_LATCHING_SIGNAL_H
#define HITTOP_CONCURRENT_LATCHING_SIGNAL_H

#include <mutex>
#include <tuple>
#include <type_traits>
#include <utility>

#include "boost/optional.hpp"
#include "boost/signals2/signal.hpp"
#include "boost/utility/in_place_factory.hpp"
#include "boost/variant.hpp"
#include "boost/variant/static_visitor.hpp"

#include "hittop/util/scope_exit.h"
#include "hittop/util/tuples.h"

namespace hittop {
namespace concurrent {

template <typename FunctionSignature> class LatchingSignal;

template <typename... Args> class LatchingSignal<void(Args...)> {
private:
  using ArgsTuple = std::tuple<typename std::decay<Args>::type...>;
  using Signal = boost::signal<void(Args...)>;

  struct IsLatchedVisitor : boost::static_visitor<bool> {
    bool operator()(const Signal &) const { return false; }
    bool operator()(const ArgsTuple &) const { return true; }
  };

  template <typename Slot> struct ConnectVisitor : boost::static_visitor<bool> {
    Slot *slot;

    void operator()(Signal &signal) const { signal.connect(std::move(*slot)); }

    void operator()(const ArgsTuple &args) const {
      util::tuples::Apply(*slot, args);
    }
  };

public:
  class DeferredInvocation {
  public:
    DeferredInvocation() = default;

    DeferredInvocation(Signal signal, ArgsTuple args)
        : signal_(std::move(signal)), args_(std::move(args)) {}

    explicit operator bool() const { return bool{handler_}; }

    void operator()() {
      if (handler_) {
        util::ScopeExit clear_state([&]() { args_ = boost::none; });
        util::tuples::Apply(std::move(handler_), std::move(*args_));
      }
    }

  private:
    Handler handler_;
    boost::optional<ArgsTuple> args_;
  };

  using result_type = void;

  LatchingSignal() = default;
  LatchingSignal(const LatchingSignal &) = delete;
  LatchingSignal &operator=(const LatchingSignal &) = delete;

  bool is_latched() const {
    std::unique_lock<std::mutex> lock(mutex_);
    return boost::apply_visitor(IsLatchedVisitor{}, state_);
  }

  template <typename Slot> void Connect(Slot &&slot) {
    std::unique_lock<std::mutex> lock(mutex_);
    boost::apply_visitor(ConnectVisitor{&slot}, state_);
  }

  // TODO - ConnectDeferred

  void operator()(Args... a) {
    std::unique_lock<std::mutex> lock(mutex_);
    if (state_.which() == 0) {
      util::ScopeExit save_args([&]() { state_ = ArgsTuple(a...); });
      auto &handler = boost::get<Handler>(state_);
      handler(a...);
    }
  }

  DeferredInvocation InvokeDeferred(Args... a) {
    DeferredInvocation result;
    {
      std::unique_lock<std::mutex> lock(mutex_);
      if (state_.which() == 0) {
        auto handler = std::move(boost::get<Handler>(state_));
        ArgsTuple args(a...);
        state_ = args;
        result = boost::in_place(std::move(handler), std::move(args));
        util::ScopeExit save_args([&]() { state_ = args; });
        util::tuples::Apply(handler, args);
      }
    }
    return result;
  }

  template <std::size_t I>
  const typename std::tuple_element<I, ArgsTuple>::type *arg() const {
    std::unique_lock<std::mutex> lock(mutex_);
    if (state_.which() == 1) {
      return std::get<I>(&boost::get<ArgsTuple>(state_));
    } else {
      return nullptr;
    }
  }

  boost::optional<ArgsTuple> args() const {
    std::unique_lock<std::mutex> lock(mutex_);
    if (state_.which() == 1) {
      return boost::get<ArgsTuple>(state_);
    } else {
      return boost::none;
    }
  }

public:
  mutable std::mutex mutex_;
  boost::variant<Handler, ArgsTuple> state_;
};

} // namespace concurrent
} // namespace hittop

#endif // HITTOP_CONCURRENT_LATCHING_SIGNAL_H
