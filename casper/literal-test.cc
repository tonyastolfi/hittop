#include "gtest/gtest.h"
#include "casper/literal.h"

#include <iterator>

using literal_a = casper::literal<'a'>;
casper::parser<literal_a> parse_a;

TEST(ParserErrorTest, Ctor) {
  std::error_condition c = make_condition(casper::OK);
  EXPECT_EQ(c.value(), casper::OK);
}

TEST(ParseLiteral, Ok) {
  std::string input = "abc";
  auto result = parse_a(input);
  EXPECT_EQ(result.condition, make_condition(casper::OK));
  EXPECT_EQ(result.next, std::next(input.begin()));
}

TEST(ParseLiteral, Incomplete) {
  std::string input = "";
  auto result = parse_a(input);
  EXPECT_EQ(result.condition, make_condition(casper::INCOMPLETE));
  EXPECT_EQ(result.next, input.begin());
}

TEST(ParseLiteral, BadChar) {
  std::string input = "cba";
  auto result = parse_a(input);
  EXPECT_EQ(result.condition, make_condition(casper::BAD_CHAR));
  EXPECT_EQ(result.next, input.begin());
}
