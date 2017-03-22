#include "hittop/parser/literal.h"
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
  EXPECT_TRUE(result.ok());
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
