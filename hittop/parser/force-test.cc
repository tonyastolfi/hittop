#include "hittop/parser/force.h"
#include "hittop/parser/force.h"

#include "gtest/gtest.h"

#include <string>

#include "hittop/parser/literal.h"
#include "hittop/parser/parser.h"
#include "hittop/parser/repeat.h"

using hittop::parser::Force;
using hittop::parser::Literal;
using hittop::parser::Parse;
using hittop::parser::Repeat;

TEST(ParseForce, OkRepeatEmpty) {
  const std::string input = "";
  auto result = Parse<Force<Repeat<Literal<'a'>>>>(input);
  EXPECT_TRUE(result.ok()) << "Actual error: "
                           << make_error_condition(result.error()).message();
}
