#include "hittop/parser/either.h"
#include "hittop/parser/either.h"

#include "gtest/gtest.h"

namespace {

using hittop::parser::Either;
using hittop::parser::Literal;
using hittop::parser::Parse;
using hittop::parser::Parser;

TEST(EitherTest, SingleChars) {
  using Grammar = Either<Literal<'a'>, Literal<'b'>, Literal<'c'>>;
  EXPECT_FALSE(Parse<Grammar>(std::string("a")).error());
  EXPECT_FALSE(Parse<Grammar>(std::string("b")).error());
  EXPECT_FALSE(Parse<Grammar>(std::string("c")).error());
  EXPECT_FALSE(!Parse<Grammar>(std::string("d")).error());
}

} // namespace
