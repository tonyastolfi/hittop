#include "gtest/gtest.h"
#include "casper/parser.h"

TEST(ParserErrorTest, Ctor) {
  std::error_condition c{casper::ParseError::BAD_CHAR};
  EXPECT_EQ(c.value(), static_cast<int>(casper::ParseError::BAD_CHAR));
}

TEST(Fallible, DefaultCtor) {
  casper::Fallible<char*> r;
  EXPECT_EQ(r.get(), nullptr);
  EXPECT_FALSE(r.error());
}

TEST(Fallible, CopyConstruct) {
  casper::Fallible<const char*> r{"foo", casper::ParseError::INCOMPLETE};
  auto s = r;
  EXPECT_EQ(s, r);
}

TEST(Fallible, CopyAssign) {
  casper::Fallible<const char*> r{"foo", casper::ParseError::INCOMPLETE};
  decltype(r) s;
  s = r;
  EXPECT_EQ(s, r);
}
