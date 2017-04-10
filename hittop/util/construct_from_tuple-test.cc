#include "hittop/util/construct_from_tuple.h"
#include "hittop/util/construct_from_tuple.h"

#include "gtest/gtest.h"

#include <string>

namespace {

using ::hittop::util::ConstructFromTuple;

TEST(ConstructFromTupleTest, StringOneArg) {
  ConstructFromTuple<std::string> s(std::make_tuple("foo"));
  EXPECT_EQ("foo", *s);
}

TEST(ConstructFromTupleTest, StringTwoArgs) {
  ConstructFromTuple<std::string> s(std::make_tuple(5, 'a'));
  EXPECT_EQ("aaaaa", *s);
}

} // namespace
