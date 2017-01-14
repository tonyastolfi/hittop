#include "hittop/parser/json_org.h"
#include "hittop/parser/json_org.h"

#include "gtest/gtest.h"

#include <stdlib.h>

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

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
