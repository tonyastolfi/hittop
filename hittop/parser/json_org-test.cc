#include "hittop/parser/json_org.h"
#include "hittop/parser/json_org.h"

#include "gtest/gtest.h"

#include <stdlib.h>

#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "boost/variant.hpp"
#include "boost/variant/get.hpp"

#include "hittop/util/first_match.h"
#include "hittop/util/test_data.h"

using hittop::parser::Parse;
using hittop::util::LoadTestData;
namespace json = hittop::parser::json_org;

TEST(ParseJson, OkBooleanTrue) {
  const char *const input = "true";
  auto result = Parse<json::Boolean>(input);
  EXPECT_FALSE(result.error());
  EXPECT_EQ(result.get(), input + 4);
}

TEST(ParseJson, OkBooleanFalse) {
  const char *const input = "false";
  auto result = Parse<json::Boolean>(input);
  EXPECT_FALSE(result.error());
  EXPECT_EQ(result.get(), input + 5);
}

TEST(ParseJson, OkBooleanWithSpace) {
  const char *const input = " false  ";
  auto result = Parse<json::Boolean>(input);
  EXPECT_FALSE(result.error());
  EXPECT_EQ(result.get(), input + 8);
}

TEST(ParseJson, OkTestDataFile) {
  auto input = LoadTestData("/hittop/parser/json_org-test-data.json");
  auto result = Parse<json::Value>(input);
  EXPECT_FALSE(result.error()) << "Actual error: " << result.error().message();
  EXPECT_EQ(result.get(), input.end());
}

using JsonNull = std::tuple<>;

template <typename Range>
inline std::string JsonUnescapeUnsafe(const Range &in) {
  std::ostringstream out;
  auto next = std::begin(in);
  auto last = std::end(in);
  for (; next != last; ++next) {
    switch (*next) {
    case '\\':
      ++next;
      switch (*next) {
      case '"':
        out << char('"');
        break;
      case '\\':
        out << char('\\');
        break;
      case '/':
        out << char('/');
        break;
      case 'b':
        out << char('\b');
        break;
      case 'f':
        out << char('\f');
        break;
      case 'n':
        out << char('\n');
        break;
      case 'r':
        out << char('\r');
        break;
      case 't':
        out << char('\t');
        break;
      default:
        // TODO!!!
        break;
      }
      break;
    default:
      out << char(*next);
    }
  }
  return out.str();
}

struct JsonValue {
public:
  using variant_type =
      boost::variant<JsonNull, bool, double, std::string,
                     std::vector<JsonValue>,
                     std::unordered_map<std::string, JsonValue>>;
  template <typename... A>
  JsonValue(A &&... a) : value_{std::forward<A>(a)...} {}

  JsonValue(JsonValue &&that) : value_(std::move(that.value_)) {}

  variant_type &operator*() { return value_; }

  const variant_type &operator*() const { return value_; }

  variant_type *operator->() { return &value_; }

  const variant_type *operator->() const { return &value_; }

  friend inline bool operator==(const JsonValue &lhs, const JsonValue &rhs) {
    return lhs.value_ == rhs.value_;
  }

private:
  variant_type value_;
};

using JsonArray = std::vector<JsonValue>;

using JsonObject = std::unordered_map<std::string, JsonValue>;

template <typename Range> std::string range_to_string(const Range &r) {
  return std::string(std::begin(r), std::end(r));
}

namespace util = hittop::util;

struct JsonValueVisitor {
private:
  JsonValue *const output_;

public:
  explicit JsonValueVisitor(JsonValue *output) : output_(output) {}

  JsonValueVisitor(const JsonValueVisitor &) = delete;
  JsonValueVisitor &operator=(const JsonValueVisitor &) = delete;

  template <typename F>
  void operator()(json::StringContents, F &&get_result) const {
    auto result = get_result();
    if (!result.error()) {
      std::clog << "Parsed string: " << range_to_string(result.get())
                << std::endl;
      **output_ = JsonUnescapeUnsafe(result.get());
    }
  }

  template <typename F> void operator()(json::Boolean, F &&get_result) const {
    auto result = get_result();
    if (!result.error()) {
      std::clog << "Parsed bool: " << range_to_string(result.get())
                << std::endl;
      **output_ = bool{*std::begin(result.get()) == 't'};
    }
  }

  template <typename F> void operator()(json::Number, F &&get_result) const {
    auto result = get_result();
    if (!result.error()) {
      std::clog << "Parsed number: " << range_to_string(result.get())
                << std::endl;
      **output_ = std::stod(range_to_string(result.get()));
    }
  }

  template <typename F> void operator()(json::Null, F &&get_result) const {
    auto result = get_result();
    if (!result.error()) {
      std::clog << "Parsed null" << std::endl;
      **output_ = JsonNull{};
    }
  }

  template <typename F> void operator()(json::Array, F &&get_result) const {
    std::vector<JsonValue> items;
    auto result = get_result([&items](json::Value, auto get_item_result) {
      JsonValue next_item;
      JsonValueVisitor item_visitor{&next_item};
      auto item_result = get_item_result(item_visitor);
      if (!item_result.error()) {
        std::clog << "Parsed array element: "
                  << range_to_string(item_result.get()) << std::endl;
        items.emplace_back(std::move(next_item));
      }
    });
    if (!result.error()) {
      std::clog << "Parsed array" << range_to_string(result.get()) << std::endl;
      **output_ = std::move(items);
    }
  }

  template <typename F> void operator()(json::Object, F &&get_result) const {
    std::unordered_map<std::string, JsonValue> object;
    auto result =
        get_result([&object](json::Property, auto get_property_result) {
          std::string name;
          get_property_result(util::FirstMatch(
              [&name](json::StringContents, auto get_name_result) {
                auto name_result = get_name_result();
                if (!name_result.error()) {
                  name = JsonUnescapeUnsafe(name_result.get());
                  std::clog << "Parsed key: " << name << std::endl;
                }
              },
              [&object, &name](json::Value, auto get_value_result) {
                JsonValue property;
                JsonValueVisitor property_visitor{&property};
                auto value_result = get_value_result(property_visitor);
                if (!value_result.error()) {
                  std::clog << "Parsed property (name=" << name
                            << "): " << range_to_string(value_result.get())
                            << ", property->which()=" << property->which()
                            << std::endl;
                  object.emplace(
                      std::make_pair(std::move(name), std::move(property)));
                }
              }));
        });
    if (!result.error()) {
      std::clog << "Parsed object: " << range_to_string(result.get())
                << ", object.size()=" << object.size() << std::endl;
      for (auto &p : object) {
        std::clog << "    " << p.first << ": (type " << p.second->which() << ")"
                  << std::endl;
      }
      **output_ = std::move(object);
      std::clog << "(*output_)->which()=" << (*output_)->which() << std::endl;
    }
  }
};

/*
template <typename Exception> struct AlwaysThrower {
  Exception ex;

  template <typename... A>
  AlwaysThrower(A &&... a) : ex(std::forward<A>(a)...) {}

  template <typename... A> void operator()(A &&...) const { throw ex; }
};

template <typename... A> AlwaysThrower<E> AlwaysThrow(A &&... a) {
  return AlwaysThrower<E>(std::forward<A>(a)...);
}
*/

TEST(ParseJson, ParseWithAction) {
  // auto input = LoadTestData("/hittop/parser/json_org-test-data.json");
  std::string input = "{\"a\": 1, \"b\": true}";
  std::clog << input << std::endl;
  JsonValue output;
  JsonValueVisitor visitor{&output};
  auto result = Parse<json::Value>(input, visitor);
  EXPECT_FALSE(result.error()) << "Actual error: " << result.error().message();
  EXPECT_EQ(result.get(), input.end());
  std::clog << "boost::get<JsonObject>(*output)['a'].which()="
            << boost::get<JsonObject>(*output)["a"]->which() << std::endl;
  std::clog << "boost::get<JsonObject>(*output)['b'].which()="
            << boost::get<JsonObject>(*output)["b"]->which() << std::endl;
  EXPECT_TRUE(boost::get<JsonObject>(*output)["a"] == JsonValue(1.0));
  EXPECT_TRUE(boost::get<JsonObject>(*output)["b"] == JsonValue(bool{true}));
}
