#ifndef HITTOP_UTIL_FIRST_MATCH_H
#define HITTOP_UTIL_FIRST_MATCH_H

namespace hittop {
namespace util {

template <typename... Args> struct IsCallableWithArgs {
  template <typename F> struct apply : IsCallable<F, Args...> {};
};

template <typename... F> class FirstMatchFunctor {
private:
  using tuple_type = std::tuple<F...>;

  tuple_type fs_;

public:
  template <typename... A>
  FirstMatchFunctor(A &&... a) : fs_(std::forward<A>(a)...) {}

#define HITTOP_UTIL_FIRST_MATCH_EXPR                                           \
  std::get<tuples::FindFirst<IsCallableWithArgs<A &&...>::template apply,      \
                             tuple_type>::index>(fs_)(std::forward<A>(a)...)

  template <typename... A>
  auto operator()(A &&... a) const -> decltype(HITTOP_UTIL_FIRST_MATCH_EXPR) {
    return HITTOP_UTIL_FIRST_MATCH_EXPR;
  }

  template <typename... A>
  auto operator()(A &&... a) -> decltype(HITTOP_UTIL_FIRST_MATCH_EXPR) {
    return HITTOP_UTIL_FIRST_MATCH_EXPR;
  }

  template <typename... A>
  auto
  operator()(A &&... a) const && -> decltype(HITTOP_UTIL_FIRST_MATCH_EXPR) {
    return HITTOP_UTIL_FIRST_MATCH_EXPR;
  }

  template <typename... A>
  auto operator()(A &&... a) && -> decltype(HITTOP_UTIL_FIRST_MATCH_EXPR) {
    return HITTOP_UTIL_FIRST_MATCH_EXPR;
  }

#undef HITTOP_UTIL_FIRST_MATCH_EXPR
};

template <typename... F> FirstMatchFunctor<F...> FirstMatch(F &&f...) {
  return FirstMatchFunction<F...>(std::forward<F>(f)...);
}

} // namesapce util
} // namesapce hittop

#endif // HITTOP_UTIL_FIRST_MATCH_H
