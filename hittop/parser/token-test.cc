#include "hittop/parser/token.h"
#include "hittop/parser/token.h"

#include "gtest/gtest.h"

#include "hittop/parser/parser.h"

#include <string>

using hittop::parser::Parse;
using hittop::parser::ParseError;

namespace {

namespace tokens {
DEFINE_TOKEN(abcd);
DEFINE_TOKEN(abcdef);
DEFINE_TOKEN(abcdx);
DEFINE_TOKEN(abcdefghi);
} // namespace tokens

const std::string &input() {
  static const std::string s = "abcdef";
  return s;
}
} // namespace

TEST(ParseToken, TokenSizes) {
  EXPECT_EQ(4, tokens::abcd::size());
  EXPECT_EQ(6, tokens::abcdef::size());
  EXPECT_EQ(5, tokens::abcdx::size());
  EXPECT_EQ(9, tokens::abcdefghi::size());
}

TEST(ParseToken, OkConsumePartial) {
  auto result = Parse<tokens::abcd>(input());
  EXPECT_FALSE(result.error());
  EXPECT_EQ(result.get(), input().begin() + 4);
}

TEST(ParseToken, OkConsumeFull) {
  auto result = Parse<tokens::abcdef>(input());
  EXPECT_FALSE(result.error());
  EXPECT_EQ(result.get(), input().end());
}

TEST(ParseToken, Incomplete) {
  auto result = Parse<tokens::abcdefghi>(input());
  EXPECT_EQ(result.error(), ParseError::INCOMPLETE);
  EXPECT_EQ(result.get(), input().end());
}

TEST(ParseToken, ErrorBadChar) {
  auto result = Parse<tokens::abcdx>(input());
  EXPECT_EQ(result.error(), ParseError::BAD_CHAR);
  EXPECT_EQ(result.get(), input().begin() + 4);
}
