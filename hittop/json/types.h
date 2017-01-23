#ifndef HITTOP_JSON_TYPES_H
#define HITTOP_JSON_TYPES_H

#include <string>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

#include "boost/variant.hpp"

namespace hittop {
namespace json {

class Value;

using Array = std::vector<Value>;
using Boolean = bool;
using Null = std::tuple<>;
using Number = double;
using String = std::string;
using Object = std::unordered_map<String, Value>;

class Value {
private:
  using VariantType =
      boost::variant<Null, Boolean, Number, String, Array, Object>;

public:
  enum struct Type { kNull, kBoolean, kNumber, kString, kArray, kObject };

  template <typename... A> Value(A &&... a) : value_{std::forward<A>(a)...} {}

  Value(Value &that) = default;
  Value(const Value &that) = default;
  Value(Value &&that) : value_(std::move(that.value_)) {}

  Value &operator=(Value &that) = default;
  Value &operator=(const Value &that) = default;

  Value &operator=(Value &&that) {
    value_ = std::move(that.value_);
    return *this;
  }

  template <typename SubType> Value &operator=(SubType &&value) {
    value_ = std::forward<SubType>(value);
    return *this;
  }

  Type type() const { return static_cast<Type>(value_.which()); }

  friend inline bool operator==(const Value &lhs, const Value &rhs) {
    return lhs.value_ == rhs.value_;
  }

  template <typename T> explicit operator T &() {
    return boost::get<T>(value_);
  }

  template <typename T> explicit operator const T &() const {
    return boost::get<T>(value_);
  }

  template <typename Visitor>
  auto Visit(Visitor &visitor) -> decltype(
      boost::apply_visitor(visitor)(std::declval<VariantType &>())) {
    return boost::apply_visitor(visitor)(value_);
  }

  template <typename Visitor>
  auto Visit(Visitor &visitor) const -> decltype(
      boost::apply_visitor(visitor)(std::declval<const VariantType &>())) {
    return boost::apply_visitor(visitor)(value_);
  }

  template <typename Visitor>
  auto Visit(const Visitor &visitor) -> decltype(
      boost::apply_visitor(visitor)(std::declval<VariantType &>())) {
    return boost::apply_visitor(visitor)(value_);
  }

  template <typename Visitor>
  auto Visit(const Visitor &visitor) const -> decltype(
      boost::apply_visitor(visitor)(std::declval<const VariantType &>())) {
    return boost::apply_visitor(visitor)(value_);
  }

private:
  VariantType value_;
};

} // namespace json
} // namespace hittop

#endif // HITTOP_JSON_TYPES_H
