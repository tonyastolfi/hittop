#ifndef HITTOP_KWARGS_KWARGS_H
#define HITTOP_KWARGS_KWARGS_H

#include <functional>

#include "boost/optional.hpp"

namespace hittop {
namespace kwargs {

// Produced by assigning a value to a keyword; usually this type doesn't need to
// be used directly by client code, but it is defined in the same namespace as
// Get(key, args...) and Consume(key, args...) for convenience sake so that ADL
// can be used (i.e., Get/Consume need not be qualified).
//
template <typename Tag, typename Value> class KeywordBinding {
public:
  explicit KeywordBinding(Value &&value) : value_(std::forward<Value>(value)) {}

  auto &&get() const { return value_; }

  Value &&consume() { return std::forward<Value>(value_); }

private:
  Value &&value_;
};

namespace internal {

// boost::optional<T> when T is a reference to a builtin type doesn't work well,
// and is forbidden when T is an rvalue reference.  In these two cases, decay
// the passed type.
//
template <typename T, bool = std::is_fundamental<std::decay_t<T>>::value ||
                             std::is_rvalue_reference<T>::value>
struct OptionalImpl {
  using type = boost::optional<T>;
};

template <typename T> struct OptionalImpl<T, true> {
  using type = boost::optional<std::decay_t<T>>;
};

template <typename T> using Optional = typename OptionalImpl<T>::type;

// Wrapper around boost::optional that allows values of types that are
// implicitly convertible to the lhs type of the caller to be bound to keyword
// arguments and transparently passed through.  For example:
//
// DEFINE_KEYWORD(key, std::string);
//
// boost::optional<std::string> value =
//   Get(key, key="a char array, not a std::string")
//
template <typename T> struct ConvertibleOptional : Optional<T> {
  template <typename... Args>
  ConvertibleOptional(Args &&... args)
      : Optional<T>(std::forward<Args>(args)...) {}

  template <typename U> operator boost::optional<U>() const & {
    boost::optional<U> u;
    u = *this;
    return u;
  }

  template <typename U> operator boost::optional<U>() && {
    boost::optional<U> u;
    u = std::move(*this);
    return u;
  }
};

} // internal

// A keyword argument that can be assigned to produce bindings and used as a key
// to retrieve the values of those bindings using Get(key, args...) and
// Consume(key, args...).  Can be used directly by client code, but
// DEFINE_KEYWORD is more concise and directly expressive of intent.
//
template <typename Tag, typename DefaultValue> struct KeywordArg {
  template <typename Value> auto operator=(Value &&value) const {
    return KeywordBinding<Tag, Value>(std::forward<Value>(value));
  }
};

// Gets the first value bound to the keyword argument 'key'.
//
// Returns rvalue and fundamental types as boost::optional of their decayed
// type.  All other types are returned by boost::optional of lvalue reference.
//
// Returns an empty boost::optional<DefaultValue> if the key doesn't appear in
// in the passed bindings.
//
template <typename Tag, typename DefaultValue>
internal::ConvertibleOptional<const DefaultValue &>
Get(const KeywordArg<Tag, DefaultValue> &) {
  return boost::none;
}

template <typename Tag, typename DefaultValue, typename Value, typename... Rest>
auto Get(const KeywordArg<Tag, DefaultValue> &,
         const KeywordBinding<Tag, Value> &first, Rest &&...)
    -> internal::ConvertibleOptional<decltype(first.get())> {
  return first.get();
}

template <typename Tag, typename DefaultValue, typename Value, typename... Rest>
auto Get(const KeywordArg<Tag, DefaultValue> &,
         KeywordBinding<Tag, Value> &&first, Rest &&...)
    -> internal::ConvertibleOptional<decltype(first.get())> {
  return first.get();
}

template <typename Tag, typename DefaultValue, typename First, typename... Rest>
auto Get(const KeywordArg<Tag, DefaultValue> &key, First &&, Rest &&... rest) {
  return Get(key, std::forward<Rest>(rest)...);
}

// Consumes the first value bound to the keyword argument 'key'.
//
// This operation will preserve rvalue references as such, allowing them to be
// moved.
//
// Returns an empty boost::optional<DefaultValue> if the key doesn't appear in
// in the passed bindings.
//
template <typename Tag, typename DefaultValue, typename Value, typename... Rest>
auto Consume(const KeywordArg<Tag, DefaultValue> &key,
             const KeywordBinding<Tag, Value> &first, Rest &&...)
    -> internal::ConvertibleOptional<decltype(first.consume())> {
  return first.consume();
}

template <typename Tag, typename DefaultValue, typename Value, typename... Rest>
auto Consume(const KeywordArg<Tag, DefaultValue> &,
             KeywordBinding<Tag, Value> &&first, Rest &&...)
    -> internal::ConvertibleOptional<decltype(first.consume())> {
  return first.consume();
}

template <typename Tag, typename DefaultValue, typename First, typename... Rest>
auto Consume(const KeywordArg<Tag, DefaultValue> &key, First &&,
             Rest &&... rest) {
  return Consume(key, std::forward<Rest>(rest)...);
}

template <typename Tag, typename DefaultValue>
internal::ConvertibleOptional<DefaultValue &&>
Consume(const KeywordArg<Tag, DefaultValue> &) {
  return boost::none;
}

} // namespace kwargs
} // namespace hittop

// Defines a new keyword argument named 'name'.  Also references an undefined
// struct type named 'name_t', so be sure not to use that identifier name, as
// they will clash.  The type retrieved by Get or Consume is usually the type
// bound directly to the keyword argument, but when the keyword is queried in an
// argument pack and not found, then an empty boost::optional<type> will be
// returned.
//
#define DEFINE_KEYWORD(name, type)                                             \
  ::hittop::kwargs::KeywordArg<struct name##_t, type> name

#endif // HITTOP_KWARGS_KWARGS_H
