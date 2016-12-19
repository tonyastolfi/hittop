#include "hittop/parser/literal.h"

#include "gtest/gtest.h"

#include <iterator>
#include <typeinfo>

using hittop::parser::Literal;
using hittop::parser::Parse;
using hittop::parser::ParseError;

using a_grammar = Literal<'a'>;
using abc_grammar = Literal<'a', 'b', 'c'>;

TEST(ParseLiteral, Ok) {
  std::string input = "abc";
  auto result = Parse<a_grammar>(input);
  EXPECT_FALSE(result.error());
  EXPECT_EQ(result.get(), std::next(input.begin()));
}

TEST(ParseLiteral, Incomplete) {
  std::string input = "";
  auto result = Parse<a_grammar>(input);
  EXPECT_EQ(result.error(), ParseError::INCOMPLETE);
  EXPECT_EQ(result.get(), input.begin());
}

TEST(ParseLiteral, BadChar) {
  std::string input = "cba";
  auto result = Parse<a_grammar>(input);
  EXPECT_EQ(result.error(), ParseError::BAD_CHAR);
  EXPECT_EQ(result.get(), input.begin());
}

namespace parse_strs {
DEFINE_PARSE_STR(abcd);
DEFINE_PARSE_STR(abcdx);
DEFINE_PARSE_STR(abcdefghi);
}

TEST(ParseLiteral, String) {
  EXPECT_EQ(4, parse_strs::abcd::size());
  EXPECT_EQ(5, parse_strs::abcdx::size());
  EXPECT_EQ(9, parse_strs::abcdefghi::size());

  std::string input = "abcdef";
  {
    auto result = Parse<abc_grammar>(input);
    EXPECT_FALSE(result.error());
    EXPECT_EQ(result.get(), input.begin() + 3);
  }
  {
    auto result = Parse<parse_strs::abcd>(input);
    EXPECT_FALSE(result.error());
    EXPECT_EQ(result.get(), input.begin() + 4);
  }
  {
    auto result = Parse<parse_strs::abcdefghi>(input);
    EXPECT_EQ(result.error(), ParseError::INCOMPLETE);
    EXPECT_EQ(result.get(), input.end());
  }
  {
    auto result = Parse<parse_strs::abcdx>(input);
    EXPECT_EQ(result.error(), ParseError::BAD_CHAR);
    EXPECT_EQ(result.get(), input.begin() + 4);
  }
}
