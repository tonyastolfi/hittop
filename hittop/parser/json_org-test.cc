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

struct JsonNull {};

struct JsonValue {
  boost::variant<JsonNull, bool, double, std::string, std::vector<JsonValue>,
                 std::unordered_map<std::string, JsonValue>>
      value;

  template <typename... A>
  JsonValue(A &&... a) : value{std::forward<A>(a)...} {}
};

template <typename Range> std::string range_to_string(const Range &r) {
  return std::string(std::begin(r), std::end(r));
}

struct JsonValueVisitor {
  JsonValue output;

  template <typename F> void operator()(json::String, F &&get_result) {
    auto result = get_result();
    if (!result.error()) {
      output.value = range_to_string(result.get());
    }
  }

  template <typename F> void operator()(json::Boolean, F &&get_result) {
    auto result = get_result();
    if (!result.error()) {
      output.value = bool{*std::begin(result.get()) == 't'};
    }
  }

  template <typename F> void operator()(json::Number, F &&get_result) {
    auto result = get_result();
    if (!result.error()) {
      output.value = std::stod(range_to_string(result.get()));
    }
  }

  template <typename F> void operator()(json::tokens::Null, F &&get_result) {
    auto result = get_result();
    if (!result.error()) {
      output.value = JsonNull{};
    }
  }

  template <typename F> void operator()(json::Array, F &&get_result) {
    std::vector<JsonValue> items;
    auto result = get_result([&items](json::Value, auto get_item_result) {
      JsonValueVisitor item_visitor;
      auto item_result = get_item_result(item_visitor);
      if (!item_result.error()) {
        items.emplace_back(std::move(item_visitor.output));
      }
    });
    if (!result.error()) {
      output.value = std::move(items);
    }
  }

  template <typename F> void operator()(json::Object, F &&get_result) {
    std::unordered_map<std::string, JsonValue> object;
    auto result = get_result([](json::Property, auto get_property_result) {
      std::string name;
      auto property_result = get_property_result(FirstMatch(
          [&name](json::String, auto get_name_result) {
            auto name_result = get_name_result();
            if (!name_result.error()) {
              name = range_to_string(name_result);
            }
          },
          [&object, &name](json::Value, auto get_value_result) {
            JsonValueVisitor property;
            auto value_result = get_value_result(property);
            if (!value_result.error()) {
              object.emplace(
                  std::make_pair(std::move(name), std::move(property.value)));
            }
          }));
    });
    if (!result.error()) {
      output.value = std::move(object);
    }
  }
};

/*
void ParseWithAction() {

  auto value_action = FirstMatch(
      [](json::String, auto get_result) {},
      [](json::Boolean, auto get_result) {
        auto result = get_result();
        if (!result.error) {
             't' == *std::begin(range)); });
      },
      [](json::Array, auto get_result) -> Fallible<std::vector<JsonValue>> {
        std::vector<JsonValue> array;
        return get_result([](json::Value, get_result) -> Fallible<void> {
                 return get_result(json_value_handler).apply([](auto value) {
                   array.push_back(value);
                 })
               })
            .apply([]() { return array; });
      });
});

Parse<json::Value>(input,

                   );
}
*/
