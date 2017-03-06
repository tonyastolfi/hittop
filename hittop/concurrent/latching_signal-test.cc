#include "hittop/concurrent/latching_signal.h"
#include "hittop/concurrent/latching_signal.h"

#include "gtest/gtest.h"

#include "boost/system/error_code.hpp"

#include <functional>
#include <string>

using hittop::concurrent::LatchingSignal;

TEST(LatchingSignalTest, ZeroArgs) {
  LatchingSignal<void()> l;
  EXPECT_FALSE(l.is_latched());
  l();
  EXPECT_TRUE(l.is_latched());
}

TEST(LatchingSignalTest, ZeroArgsThruRef) {
  LatchingSignal<void()> l;
  EXPECT_FALSE(l.is_latched());
  std::ref(l)();
  EXPECT_TRUE(l.is_latched());
}

TEST(LatchingSignalTest, TwoArgsThruRef) {
  LatchingSignal<void(const boost::system::error_code &, std::string *)> l;
  EXPECT_FALSE(l.is_latched());
  auto wrapper = std::ref(l);
  boost::optional<decltype(wrapper)> opt;
  opt = wrapper;
  (*opt)(boost::system::error_code{}, nullptr);
  EXPECT_TRUE(l.is_latched());
}
