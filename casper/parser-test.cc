#include "gtest/gtest.h"
#include "casper/parser.h"

TEST(ParserErrorTest, Ctor) {
  std::error_condition c = make_condition(casper::OK);
  EXPECT_EQ(c.value(), casper::OK);
}

TEST(ParseResult, DefaultCtor) {
  casper::ParseResult<char*> r;
  EXPECT_EQ(r.next, nullptr);
  EXPECT_EQ(r.condition, make_condition(casper::OK));
}

TEST(ParseResult, CopyConstruct) {
  casper::ParseResult<char*> r{"foo", casper::INCOMPLETE};
  auto s = r;
  EXPECT_EQ(s, r);
}

TEST(ParseResult, CopyAssign) {
  casper::ParseResult<char*> r{"foo", casper::INCOMPLETE};
  decltype(r) s;
  s = r;
  EXPECT_EQ(s, r);
}
