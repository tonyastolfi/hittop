#ifndef HITTOP_UTIL_CONSTRUCT_FROM_TUPLE_H
#define HITTOP_UTIL_CONSTRUCT_FROM_TUPLE_H

#include <tuple>
#include <type_traits>

namespace hittop {
namespace util {

template <typename T> class ConstructFromTuple {
public:
  template <typename... Args,
            std::size_t Arity = std::tuple_size<std::tuple<Args...>>::value,
            typename IndexSequence = std::make_index_sequence<Arity>>
  explicit ConstructFromTuple(std::tuple<Args...> &&args)
      : ConstructFromTuple(std::move(args), IndexSequence{}) {}

  template <typename... Args, std::size_t... I>
  ConstructFromTuple(std::tuple<Args...> &&args,
                     std::index_sequence<I...> indices)
      : value_(std::move(std::get<I>(args))...) {}

  template <typename... Args,
            std::size_t Arity = std::tuple_size<std::tuple<Args...>>::value,
            typename IndexSequence = std::make_index_sequence<Arity>>
  explicit ConstructFromTuple(const std::tuple<Args...> &args)
      : ConstructFromTuple(args, IndexSequence{}) {}

  template <typename... Args, std::size_t... I>
  ConstructFromTuple(const std::tuple<Args...> &args,
                     std::index_sequence<I...> indices)
      : value_(std::get<I>(args)...) {}

  template <typename... Args,
            std::size_t Arity = std::tuple_size<std::tuple<Args...>>::value,
            typename IndexSequence = std::make_index_sequence<Arity>>
  explicit ConstructFromTuple(std::tuple<Args...> &args)
      : ConstructFromTuple(args, IndexSequence{}) {}

  template <typename... Args, std::size_t... I>
  ConstructFromTuple(std::tuple<Args...> &args,
                     std::index_sequence<I...> indices)
      : value_(std::get<I>(args)...) {}

  // You can treat this type like a pointer to 'T'
  T &operator*() { return value_; }
  const T &operator*() const { return value_; }
  std::decay_t<T> *operator->() { return &value_; }
  const std::decay_t<T> *operator->() const { return &value_; }

  // Or if you'd rather have named methods...
  T &get() { return value_; }
  const T &get() const { return value_; }
  std::decay_t<T> *get_ptr() { return &value_; }
  const std::decay_t<T> *get_ptr() const { return &value_; }

private:
  T value_;
};

} // namespace util
} // namespace hittop

#endif // HITTOP_UTIL_CONSTRUCT_FROM_TUPLE_H
