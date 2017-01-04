#include "hittop/parser/json_org.h"
#include "hittop/parser/json_org.h"

#include "gtest/gtest.h"

using hittop::parser::Parse;
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
