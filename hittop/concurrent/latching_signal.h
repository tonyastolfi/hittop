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

using Connection = boost::signals2::connection;
using DeferredConnection = std::function<boost::optional<Connection>()>;

template <typename... Args> class LatchingSignal<void(Args...)> {
private:
  using ArgsTuple = std::tuple<std::decay_t<Args>...>;

  template <std::size_t I> using ArgType = std::tuple_element_t<I, ArgsTuple>;

  using Signal = boost::signals2::signal<void(Args...)>;

  using State = boost::variant<Signal, ArgsTuple>;

  struct IsLatchedVisitor : boost::static_visitor<bool> {
    bool operator()(const Signal &) const { return false; }
    bool operator()(const ArgsTuple &) const { return true; }
  };

  template <std::size_t I>
  struct GetSingleArgVisitor : boost::static_visitor<const ArgType<I> *> {
    const ArgType<I> *operator()(const Signal &) const { return nullptr; }
    const ArgType<I> *operator()(const ArgsTuple &args) const {
      return &std::get<I>(args);
    }
  };

  struct GetArgsVisitor : boost::static_visitor<const ArgsTuple *> {
    const ArgsTuple *operator()(const Signal &) const { return nullptr; }
    const ArgsTuple *operator()(const ArgsTuple &args) const { return &args; }
  };

  template <typename Slot>
  struct ConnectVisitor : boost::static_visitor<boost::optional<Connection>> {
    Slot &&slot;

    boost::optional<Connection> operator()(Signal &signal) const {
      return signal.connect(std::forward<Slot>(slot));
    }

    boost::optional<Connection> operator()(const ArgsTuple &args) const {
      util::tuples::Apply(std::forward<Slot>(slot), args);
      return boost::none;
    }
  };

  template <typename Slot>
  struct DeferredConnectVisitor : boost::static_visitor<DeferredConnection> {
    Slot &&slot;

    DeferredConnection operator()(Signal &signal) const {
      return [connection = signal.connection(std::forward<Slot>(slot))]()
          ->DeferredConnection {
        return connection;
      };
    }

    DeferredConnection operator()(const ArgsTuple &args) const {
      return [ args, slot = std::forward<Slot>(slot) ]() {
        util::tuples::Apply(std::forward<Slot>(slot), args);
        return boost::none;
      };
    }
  };

public:
  class DeferredInvocation {
  public:
    DeferredInvocation() = default;

    DeferredInvocation(Signal signal, ArgsTuple args)
        : signal_(std::move(signal)), args_(std::move(args)) {}

    explicit operator bool() const { return bool{signal_}; }

    void operator()() {
      if (signal_) {
        util::ScopeExit clear_state([&]() {
          signal_ = boost::none;
          args_ = boost::none;
        });
        util::tuples::Apply(std::move(signal_), std::move(*args_));
      }
    }

  private:
    boost::optional<Signal> signal_;
    boost::optional<ArgsTuple> args_;
  };

  using result_type = void;

  // Construct an unlatched object.
  LatchingSignal() { state_.emplace(); }

  // Construct in a latched state.
  template <typename... A>
  explicit LatchingSignal(A &&... a)
      : state_(boost::in_place(ArgsTuple(std::forward<A>(a)...))) {}

  // Move construct/assign.
  LatchingSignal(LatchingSignal &&) = default;
  LatchingSignal &operator=(LatchingSignal &&) = default;

  // Disable copying.
  LatchingSignal(const LatchingSignal &) = delete;
  LatchingSignal &operator=(const LatchingSignal &) = delete;

  bool is_latched() const {
    std::unique_lock<std::mutex> lock(mutex_);
    return boost::apply_visitor(IsLatchedVisitor{}, *state_);
  }

  // Connects the slot to the signal if not yet latched; if latched, invokes the
  // slot immediately with the stored args.
  template <typename Slot> boost::optional<Connection> Connect(Slot &&slot) {
    std::unique_lock<std::mutex> lock(mutex_);
    return boost::apply_visitor(ConnectVisitor<Slot>{&slot}, *state_);
  }

  template <typename Slot> DeferredConnection ConnectDeferred(Slot &&slot) {
    std::unique_lock<std::mutex> lock(mutex_);
    return boost::apply_visitor(
        DeferredConnectVisitor<Slot>{std::forward<Slot>(slot)}, *state_);
  }

  template <typename... A> bool operator()(A &&... a) {
    std::unique_lock<std::mutex> lock(mutex_);
    // Intentionally not using visitor here so that we don't need to shuttle the
    // args around.
    if (state_->which() == 0) {
      Signal signal = std::move(boost::get<Signal>(*state_));
      // TODO - what if constructing the ArgsTuple throws?
      state_.emplace(ArgsTuple(std::forward<A>(a)...));
      util::tuples::Apply(std::move(signal), boost::get<ArgsTuple>(*state_));
      return true;
    } else {
      return false;
    }
  }

  template <typename... A> DeferredInvocation InvokeDeferred(A &&... a) {
    std::unique_lock<std::mutex> lock(mutex_);
    // Intentionally not using visitor here so that we don't need to shuttle the
    // args around.
    if (state_->which() == 0) {
      Signal signal = std::move(boost::get<Signal>(*state_));
      // TODO - what if constructing the ArgsTuple throws?
      state_.emplace(ArgsTuple(std::forward<A>(a)...));
      return DeferredInvocation(std::move(signal),
                                boost::get<ArgsTuple>(*state_));
    } else {
      return DeferredInvocation{};
    }
  }

  template <std::size_t I> const ArgType<I> *arg() const {
    std::unique_lock<std::mutex> lock(mutex_);
    return boost::apply_visitor(GetSingleArgVisitor<I>{}, *state_);
  }

  const ArgsTuple *args() const {
    std::unique_lock<std::mutex> lock(mutex_);
    return boost::apply_visitor(GetArgsVisitor{}, *state_);
  }

  void reset() {
    std::unique_lock<std::mutex> lock(mutex_);
    state_.emplace();
  }

private:
  mutable std::mutex mutex_;
  // TODO - use aligned storage here
  boost::optional<State> state_;
};

} // namespace concurrent
} // namespace hittop

#endif // HITTOP_CONCURRENT_LATCHING_SIGNAL_H
