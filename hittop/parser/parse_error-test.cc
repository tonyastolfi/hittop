#include "hittop/parser/parse_error.h"
#include "hittop/parser/parse_error.h"

#include <memory>
#include <utility>

#include "gtest/gtest.h"

using hittop::parser::ParseError;

TEST(ParserErrorTest, Ctor) {
  std::error_condition c{ParseError::BAD_CHAR};
  EXPECT_EQ(c.value(), static_cast<int>(ParseError::BAD_CHAR));
  EXPECT_EQ(c, ParseError::BAD_CHAR);
  EXPECT_NE(c, ParseError::INCOMPLETE);
}
