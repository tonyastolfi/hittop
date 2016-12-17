#include "casper/literal.h"
#include "gtest/gtest.h"

#include <iterator>
#include <typeinfo>

using casper::Literal;
using casper::Parse;
using casper::ParseError;

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

namespace tokens {
DEFINE_TOKEN(abcd);
DEFINE_TOKEN(abcdx);
DEFINE_TOKEN(abcdefghi);
}

TEST(ParseLiteral, String) {
  EXPECT_EQ(4, tokens::abcd::size());
  EXPECT_EQ(5, tokens::abcdx::size());
  EXPECT_EQ(9, tokens::abcdefghi::size());

  std::string input = "abcdef";
  {
    auto result = Parse<abc_grammar>(input);
    EXPECT_FALSE(result.error());
    EXPECT_EQ(result.get(), input.begin() + 3);
  }
  {
    auto result = Parse<tokens::abcd>(input);
    EXPECT_FALSE(result.error());
    EXPECT_EQ(result.get(), input.begin() + 4);
  }
  {
    auto result = Parse<tokens::abcdefghi>(input);
    EXPECT_EQ(result.error(), ParseError::INCOMPLETE);
    EXPECT_EQ(result.get(), input.end());
  }
  {
    auto result = Parse<tokens::abcdx>(input);
    EXPECT_EQ(result.error(), ParseError::BAD_CHAR);
    EXPECT_EQ(result.get(), input.begin() + 4);
  }
}
