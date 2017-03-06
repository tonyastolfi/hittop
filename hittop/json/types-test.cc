#include "hittop/json/types.h"
#include "hittop/json/types.h"

#include "gtest/gtest.h"

#include "hittop/util/first_match.h"

using hittop::util::FirstMatch;

namespace json = hittop::json;

namespace {
const char *GetTypeName(const json::Value &v) {
  return v.Visit(FirstMatch([](const json::Null &) { return "null"; },
                            [](const json::Number &) { return "number"; },
                            [](const json::String &) { return "string"; },
                            [](const json::Array &) { return "array"; },
                            [](const json::Object &) { return "object"; }));
}

bool IsScalar(const json::Value &v) {
  return v.Visit(FirstMatch([](const json::Array &) { return false; },
                            [](const json::Object &) { return false; },
                            [](const auto &) { return true; }));
}
} // namespace

TEST(JsonTypesTest, TestVisit) {
  EXPECT_EQ("null", GetTypeName(json::Null{}));
  EXPECT_EQ("number", GetTypeName(json::Number{3.14}));
  EXPECT_EQ("string", GetTypeName(json::String{}));
  EXPECT_EQ("array", GetTypeName(json::Array{}));
  EXPECT_EQ("object", GetTypeName(json::Object{}));

  EXPECT_TRUE(IsScalar(json::Null{}));
  EXPECT_TRUE(IsScalar(json::Boolean{}));
  EXPECT_TRUE(IsScalar(json::Number{}));
  EXPECT_TRUE(IsScalar(json::String{}));
  EXPECT_FALSE(IsScalar(json::Array{}));
  EXPECT_FALSE(IsScalar(json::Object{}));
}
