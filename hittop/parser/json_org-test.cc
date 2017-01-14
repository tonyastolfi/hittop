#include "hittop/parser/json_org.h"
#include "hittop/parser/json_org.h"

#include "gtest/gtest.h"

#include <stdlib.h>

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

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

TEST(ParseJson, OkTestDataFile) {
  const char *const TEST_SRCDIR = std::getenv("TEST_SRCDIR");
  ASSERT_TRUE(TEST_SRCDIR);

  const char *const TEST_WORKSPACE = std::getenv("TEST_WORKSPACE");
  ASSERT_TRUE(TEST_SRCDIR);

  const std::string path = std::string(TEST_SRCDIR) + "/" + TEST_WORKSPACE +
                           "/hittop/parser/json_org-test-data.json";
  std::clog << path << std::endl;

  std::ostringstream oss;
  {
    std::ifstream ifs(path);
    ASSERT_TRUE(ifs.good());
    oss << ifs.rdbuf();
  }
  const std::string input = oss.str();
  auto result = Parse<json::Value>(input);
  EXPECT_FALSE(result.error()) << "Actual error: " << result.error().message();
  EXPECT_EQ(result.get(), input.end());
}
