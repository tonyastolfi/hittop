#ifndef HITTOP_UTIL_TAIL_CALL_H
#define HITTOP_UTIL_TAIL_CALL_H

#include <functional>
#include <type_traits>

namespace hittop {
namespace util {

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

  void swap(TailCall &that) { k_.swap(that.k_); }

private:
  std::function<TailCall()> k_;
};

// Fully unwinds the passed deferred tail call.
//
template <typename TC> void unwind(TC &&arg) {
  TailCall tc = std::forward<TC>(arg);
  while (tc) {
    tc = tc();
  }
}

} // namespace util
} // namespace hittop

#endif // HITTOP_UTIL_TAIL_CALL_H
