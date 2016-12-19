#include "hittop/parser/parser.h"

#include <memory>
#include <utility>

#include "gtest/gtest.h"

using hittop::parser::Fallible;
using hittop::parser::ParseError;

TEST(ParserErrorTest, Ctor) {
  std::error_condition c{ParseError::BAD_CHAR};
  EXPECT_EQ(c.value(), static_cast<int>(ParseError::BAD_CHAR));
}
