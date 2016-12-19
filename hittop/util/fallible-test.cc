#include "hittop/util/fallible.h"

#include <memory>
#include <utility>

#include "gtest/gtest.h"
#include "hittop/parser/parse_error.h"

using hittop::util::Fallible;
using hittop::parser::ParseError;

TEST(Fallible, DefaultCtor) {
  Fallible<int> r;
  EXPECT_EQ(r.get(), int{});
  EXPECT_FALSE(r.error());
}

TEST(Fallible, ValueCtor) {
  Fallible<int> r = 5;
  EXPECT_EQ(r.get(), 5);
  EXPECT_FALSE(r.error());
}

TEST(Fallible, ImplicitConvertValue) {
  const auto fence = [](Fallible<int> value) {
    return value;
  };
  Fallible<int> r = fence(5);
  EXPECT_EQ(r.get(), 5);
  EXPECT_FALSE(r.error());
}

TEST(Fallible, CopyConstruct) {
  Fallible<int> r{5, ParseError::INCOMPLETE};
  auto s = r;
  EXPECT_EQ(s, r);
  EXPECT_EQ(s.get(), 5);
}

TEST(Fallible, CopyAssign) {
  Fallible<int> r{5, ParseError::INCOMPLETE};
  decltype(r) s;
  EXPECT_EQ(std::addressof(s = r), &s);
  EXPECT_EQ(s, r);
  EXPECT_EQ(s.get(), 5);
}

TEST(Fallible, MoveAndConsume) {
  Fallible<std::unique_ptr<int>> r = std::make_unique<int>(5);
  decltype(r) s;
  EXPECT_EQ(std::addressof(s = std::move(r)), &s);
  auto t = std::move(s);
  auto i = t.consume();
  EXPECT_EQ(*i, 5);
  EXPECT_EQ(r.get(), nullptr);
  EXPECT_EQ(s.get(), nullptr);
}
