#include "hittop/concurrent/latch.h"
#include "hittop/concurrent/latch.h"

#include "gtest/gtest.h"

#include "boost/system/error_code.hpp"

#include <functional>
#include <string>

using hittop::concurrent::Latch;

TEST(LatchTest, ZeroArgs) {
  Latch<void()> l([]() {});
  EXPECT_FALSE(l.is_latched());
  l();
  EXPECT_TRUE(l.is_latched());
}

TEST(LatchTest, ZeroArgsThruRef) {
  Latch<void()> l([]() {});
  EXPECT_FALSE(l.is_latched());
  std::ref(l)();
  EXPECT_TRUE(l.is_latched());
}

TEST(LatchTest, TwoArgsThruRef) {
  Latch<void(const boost::system::error_code &, std::string *)> l(
      [](const boost::system::error_code &, std::string *) {});
  EXPECT_FALSE(l.is_latched());
  auto wrapper = std::ref(l);
  boost::optional<decltype(wrapper)> opt;
  opt = wrapper;
  (*opt)(boost::system::error_code{}, nullptr);
  EXPECT_TRUE(l.is_latched());
}
