#include "hittop/json/parser.h"
#include "hittop/json/parser.h"

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
#include "hittop/util/type_traits.h"

using hittop::parser::Parse;
using hittop::util::LoadTestData;
using hittop::util::ConstRangeIterator;

namespace json = hittop::json;
namespace grammar = hittop::json::grammar;

TEST(ParseJson, OkBooleanTrue) {
  const char *const input = "true";
  auto result = Parse<grammar::Boolean>(input);
  EXPECT_FALSE(result.error());
  EXPECT_EQ(result.get(), input + 4);
}

TEST(ParseJson, OkBooleanFalse) {
  const char *const input = "false";
  auto result = Parse<grammar::Boolean>(input);
  EXPECT_FALSE(result.error());
  EXPECT_EQ(result.get(), input + 5);
}

TEST(ParseJson, OkBooleanWithSpace) {
  const char *const input = " false  ";
  auto result = Parse<grammar::Boolean>(input);
  EXPECT_FALSE(result.error());
  EXPECT_EQ(result.get(), input + 8);
}

TEST(ParseJson, OkTestDataFile) {
  auto input = LoadTestData("/hittop/json/test-data.json");
  auto result = Parse<grammar::Value>(input);
  EXPECT_FALSE(result.error()) << "Actual error: " << result.error().message();
  EXPECT_EQ(result.get(), input.end());
}

TEST(ParseJson, ParseSimpleObject) {
  std::string input = "{\"a\": 1, \"b\": true}";
  json::Value output;
  json::ValueParseVisitor visitor{&output};
  auto result = Parse<grammar::Value>(input, visitor);
  EXPECT_FALSE(result.error()) << "Actual error: " << result.error().message();
  EXPECT_EQ(result.get(), input.end());
  EXPECT_EQ(output.type(), json::Value::Type::kObject);
  EXPECT_TRUE(static_cast<json::Object &>(output)["a"] == json::Number(1));
  EXPECT_TRUE(static_cast<json::Object &>(output)["b"] == json::Boolean(true));
}

TEST(ParseJson, ParseComplexObject) {
  auto input = LoadTestData("/hittop/json/test-data.json");
  auto result = json::ParseValue(input);
  EXPECT_FALSE(result.error()) << "Actual error: " << result.error().message();

  json::Value output;
  ConstRangeIterator<decltype(input)>::type last_char_parsed;
  std::tie(output, last_char_parsed) = result.consume();
  EXPECT_EQ(last_char_parsed, input.end());

  auto &object = static_cast<json::Object &>(output);

  EXPECT_EQ(object.size(), 1);
  EXPECT_TRUE(object["web-app"].type() == json::Value::Type::kObject);

  auto &web_app = static_cast<json::Object &>(object["web-app"]);

  EXPECT_EQ(web_app.size(), 3);
  EXPECT_TRUE(web_app["servlet"].type() == json::Value::Type::kArray);

  const auto &servlet = static_cast<const json::Array &>(web_app["servlet"]);

  EXPECT_EQ(servlet.size(), 5);
}
