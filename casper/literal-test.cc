#include "gtest/gtest.h"
#include "casper/literal.h"

#include <iterator>

using literal_a = casper::literal<'a'>;
casper::parser<literal_a> parse_a;

TEST(ParseLiteral, Ok) {
  std::string input = "abc";
  auto result = parse_a(input);
  EXPECT_FALSE(result.error());
  EXPECT_EQ(result.get(), std::next(input.begin()));
}

TEST(ParseLiteral, Incomplete) {
  std::string input = "";
  auto result = parse_a(input);
  EXPECT_EQ(result.error(),
            make_error_condition(casper::ParseError::INCOMPLETE));
  EXPECT_EQ(result.get(), input.begin());
}

TEST(ParseLiteral, BadChar) {
  std::string input = "cba";
  auto result = parse_a(input);
  EXPECT_EQ(result.error(), make_error_condition(casper::ParseError::BAD_CHAR));
  EXPECT_EQ(result.get(), input.begin());
}
