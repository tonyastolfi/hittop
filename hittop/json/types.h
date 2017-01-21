#ifndef HITTOP_JSON_TYPES_H
#define HITTOP_JSON_TYPES_H

#include <string>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>

#include "boost/variant.hpp"

namespace hittop {
namespace json {

struct Value;

using Array = std::vector<Value>;
using Boolean = bool;
using Null = std::tuple<>;
using Number = double;
using String = std::string;
using Object = std::unordered_map<String, Value>;

struct Value {
public:
  using VariantType =
      boost::variant<Null, Boolean, Number, String, Array, Object>;

  template <typename... A> Value(A &&... a) : value_{std::forward<A>(a)...} {}

  Value(Value &&that) : value_(std::move(that.value_)) {}

  Value &operator=(Value &&that) {
    value_ = std::move(that.value_);
    return *this;
  }

  VariantType &operator*() { return value_; }

  const VariantType &operator*() const { return value_; }

  VariantType *operator->() { return &value_; }

  const VariantType *operator->() const { return &value_; }

  friend inline bool operator==(const Value &lhs, const Value &rhs) {
    return lhs.value_ == rhs.value_;
  }

private:
  VariantType value_;
};

} // namespace json
} // namespace hittop

#endif // HITTOP_JSON_TYPES_H
