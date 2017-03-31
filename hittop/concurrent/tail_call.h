#ifndef HITTOP_CONCURRENT_TAIL_CALL_H
#define HITTOP_CONCURRENT_TAIL_CALL_H

#include <functional>
#include <type_traits>

namespace hittop {
namespace concurrent {

class TailCall {
public:
  TailCall() : k_{} {}

  template <typename F, typename = std::enable_if_t<std::is_same<
                            decltype(std::declval<F>()()), TailCall>::value>>
  /* intentionally implicit */ TailCall(F &&f) : k_{std::forward<F>(f)} {}

  template <typename F, typename = std::enable_if_t<!std::is_same<
                            decltype(std::declval<F>()()), TailCall>::value>,
            typename = void>
  /* intentionally implicit */ TailCall(F &&f)
      : k_([f = std::forward<F>(f)]() {
          f();
          return TailCall{};
        }) {}

  TailCall(const TailCall &that) : k_(that.k_) {}

  TailCall(TailCall &that) : k_(that.k_) {}

  TailCall(TailCall &&that) : k_(std::move(that.k_)) {}

  TailCall &operator=(const TailCall &that) {
    k_ = that.k_;
    return *this;
  }

  TailCall &operator=(TailCall &&that) {
    k_ = std::move(that.k_);
    return *this;
  }

  explicit operator bool() const { return bool{k_}; }

  TailCall operator()() & { return k_(); }
  TailCall operator()() && { return k_(); }
  TailCall operator()() const & { return k_(); }
  TailCall operator()() const && { return k_(); }

private:
  std::function<TailCall()> k_;
};

} // namespace concurrent
} // namespace hittop

#endif // HITTOP_CONCURRENT_TAIL_CALL_H
