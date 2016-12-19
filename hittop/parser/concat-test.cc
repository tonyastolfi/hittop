#include "hittop/parser/concat.h"

#include <string>

#include "boost/range/as_literal.hpp"
#include "casper/literal.h"
#include "casper/parser.h"
#include "gtest/gtest.h"

using boost::as_literal;
using casper::Concat;
using casper::Literal;
using casper::Parse;
using casper::ParseError;

using ab_grammar = Concat<Literal<'a'>, Literal<'b'>>;

namespace {
const char kGood[] = "abc";
const char kExact[] = "ab";
const char kPartial[] = "a";
const char kEmpty[] = "";
const char kBad0[] = "xabc";
const char kBad1[] = "axbc";
const char kMultiPart[] = "abababababa";
} // namespace

TEST(ParseConcat, OkPrefix) {
  auto result = Parse<ab_grammar>(as_literal(kGood));
  EXPECT_FALSE(result.error());
  EXPECT_EQ(result.get(), &kGood[2]);
}

TEST(ParseConcat, OkFull) {
  auto result = Parse<ab_grammar>(as_literal(kExact));
  EXPECT_FALSE(result.error());
  EXPECT_EQ(result.get(), &kExact[2]);
}

TEST(ParseConcat, IncompleteEmptyInput) {
  auto result = Parse<ab_grammar>(as_literal(kEmpty));
  EXPECT_EQ(result.error(), ParseError::INCOMPLETE);
  EXPECT_EQ(result.get(), &kEmpty[0]);
}

TEST(ParseConcat, IncompleteFirstConsumesAll) {
  auto result = Parse<ab_grammar>(as_literal(kPartial));
  EXPECT_EQ(result.error(), ParseError::INCOMPLETE);
  EXPECT_EQ(result.get(), &kPartial[1]);
}

TEST(ParseConcat, BadCharFirst) {
  auto result = Parse<ab_grammar>(as_literal(kBad0));
  EXPECT_EQ(result.error(), ParseError::BAD_CHAR);
  EXPECT_EQ(result.get(), &kBad0[0]);
}

TEST(ParseConcat, BadCharSecond) {
  auto result = Parse<ab_grammar>(as_literal(kBad1));
  EXPECT_EQ(result.error(), ParseError::BAD_CHAR);
  EXPECT_EQ(result.get(), &kBad1[1]);
}

TEST(ParseConcat, MoreThan2Parts) {
  auto result =
      Parse<Concat<ab_grammar, ab_grammar, ab_grammar>>(as_literal(kMultiPart));
  EXPECT_FALSE(result.error());
  EXPECT_EQ(result.get(), &kMultiPart[6]);
}
