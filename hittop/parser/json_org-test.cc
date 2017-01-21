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

namespace util = hittop::util;

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
