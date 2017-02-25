#ifndef HITTOP_UTIL_FIRST_MATCH_H
#define HITTOP_UTIL_FIRST_MATCH_H

#include <type_traits>

#include "hittop/util/is_callable.h"
#include "hittop/util/tuples.h"

namespace hittop {
namespace util {

template <typename... Args> struct IsCallableWithArgs {
  template <typename F> struct apply : IsCallable<F, Args...> {};
};

template <typename... F> class FirstMatchFunctor {
private:
  using tuple_type = std::tuple<F...>;

  template <typename... A>
  using FirstMatchIndex =
      tuples::FindFirst<IsCallableWithArgs<A &&...>::template apply,
                        tuple_type>;

  template <typename Match, typename... A> struct ResultOf {
    using type = decltype(std::get<Match::index>(std::declval<tuple_type>())(
        std::declval<A>()...));
  };

  tuple_type fs_;

public:
  template <typename... A>
  FirstMatchFunctor(A &&... a) : fs_(std::forward<A>(a)...) {}

#define HITTOP_UTIL_FIRST_MATCH_RESULT_TYPE_EXPR                               \
  typename ResultOf<std::enable_if_t<(FirstMatchIndex<A...>::index) <          \
                                         (std::tuple_size<tuple_type>::value), \
                                     FirstMatchIndex<A...>>,                   \
                    A...>::type

#define HITTOP_UTIL_FIRST_MATCH_EXPR                                           \
  std::get<FirstMatchIndex<A...>::index>(fs_)(std::forward<A>(a)...)

  template <typename... A>
  auto
  operator()(A &&... a) const & -> HITTOP_UTIL_FIRST_MATCH_RESULT_TYPE_EXPR {
    return HITTOP_UTIL_FIRST_MATCH_EXPR;
  }

  template <typename... A>
  auto operator()(A &&... a) & -> HITTOP_UTIL_FIRST_MATCH_RESULT_TYPE_EXPR {
    return HITTOP_UTIL_FIRST_MATCH_EXPR;
  }

  template <typename... A>
  auto
  operator()(A &&... a) const && -> HITTOP_UTIL_FIRST_MATCH_RESULT_TYPE_EXPR {
    return HITTOP_UTIL_FIRST_MATCH_EXPR;
  }

  template <typename... A>
  auto operator()(A &&... a) && -> HITTOP_UTIL_FIRST_MATCH_RESULT_TYPE_EXPR {
    return HITTOP_UTIL_FIRST_MATCH_EXPR;
  }

#undef HITTOP_UTIL_FIRST_MATCH_EXPR
};

template <typename... F> auto FirstMatch(F &&... f) {
  return FirstMatchFunctor<std::decay_t<F>...>(std::forward<F>(f)...);
}

template <typename... F> auto FirstMatchRef(F &&... f) {
  return FirstMatchFunctor<F...>(std::forward<F>(f)...);
}

} // namesapce util
} // namesapce hittop

#endif // HITTOP_UTIL_FIRST_MATCH_H
