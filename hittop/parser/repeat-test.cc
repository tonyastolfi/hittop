#include "hittop/parser/repeat.h"
#include "hittop/parser/repeat.h"

#include <string>

#include "boost/range/as_literal.hpp"

#include "gtest/gtest.h"

#include "hittop/parser/concat.h"
#include "hittop/parser/literal.h"
#include "hittop/parser/parser.h"

using boost::as_literal;
using hittop::parser::Concat;
using hittop::parser::Repeat;
using hittop::parser::Literal;
using hittop::parser::Parse;
using hittop::parser::ParseError;

using ab_grammar = Concat<Literal<'a'>, Literal<'b'>>;

namespace {
const char kGood[] = "abc";
const char kExact[] = "ab";
const char kPartial[] = "a";
const char kEmpty[] = "";
const char kBad0[] = "xabc";
const char kBad1[] = "axbc";
const char kMultiPart[] = "abababababa";
const char kMultiPartDone[] = "abababababax";
} // namespace

TEST(ParseRepeat, OkSingle) {
  auto result = Parse<Repeat<ab_grammar>>(as_literal(kGood));
  EXPECT_FALSE(result.error());
  EXPECT_EQ(result.get(), &kGood[2]);
}

TEST(ParseRepeat, IncompleteSingle) {
  auto result = Parse<Repeat<ab_grammar>>(as_literal(kExact));
  // In this case, even though we consume exactly the input, we expect the
  //  parser to return INCOMPLETE because, who knows, maybe what follows is
  //  another valid parse of the repeated grammar.
  EXPECT_EQ(result.error(), ParseError::INCOMPLETE);
  EXPECT_EQ(result.get(), &kExact[2]);
}

TEST(ParseRepeat, IncompleteEmptyInput) {
  auto result = Parse<Repeat<ab_grammar>>(as_literal(kEmpty));
  EXPECT_EQ(result.error(), ParseError::INCOMPLETE);
  EXPECT_EQ(result.get(), &kEmpty[0]);
}

TEST(ParseRepeat, IncompleteZero) {
  auto result = Parse<Repeat<ab_grammar>>(as_literal(kPartial));
  EXPECT_EQ(result.error(), ParseError::INCOMPLETE);
  EXPECT_EQ(result.get(), &kPartial[1]);
}

TEST(ParseRepeat, OkZeroErrorOnFirstChar) {
  auto result = Parse<Repeat<ab_grammar>>(as_literal(kBad0));
  // Failure to parse a single instance of the repeated grammar is considered
  //  success.
  EXPECT_FALSE(result.error());
  EXPECT_EQ(result.get(), &kBad0[0]);
}

TEST(ParseRepeat, OkZeroErrorOnSecondChar) {
  auto result = Parse<Repeat<ab_grammar>>(as_literal(kBad1));
  // Repeat never fails other than INCOMPLETE.
  EXPECT_FALSE(result.error());
  EXPECT_EQ(result.get(), &kBad1[0]);
}

TEST(ParseRepeat, IncompleteMany) {
  auto result = Parse<Repeat<ab_grammar>>(as_literal(kMultiPart));
  EXPECT_EQ(result.error(), ParseError::INCOMPLETE);
  EXPECT_EQ(result.get(), &kMultiPart[11]);
}

TEST(ParseRepeat, OkMany) {
  auto result = Parse<Repeat<ab_grammar>>(as_literal(kMultiPartDone));
  EXPECT_FALSE(result.error());
  EXPECT_EQ(result.get(), &kMultiPartDone[10]);
}
