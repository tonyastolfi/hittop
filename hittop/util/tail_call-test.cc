#include "hittop/util/tail_call.h"
#include "hittop/util/tail_call.h"

#include "gtest/gtest.h"

namespace {

using ::hittop::util::TailCall;

template <typename K>
TailCall RecursiveSumTC(long long count, K &&k, long long sum = 0) {
  if (count == 0) {
    std::forward<K>(k)(sum);
    return {};
  }
  return [&k, count, sum]() {
    return RecursiveSumTC(count - 1, std::forward<K>(k), sum + count);
  };
}

TEST(TailCallTest, RecursiveSum) {
  const long long kInput = 10000000;
  long long ans;
  TailCall tc = RecursiveSumTC(kInput, [&ans](long long a) { ans = a; });
  while (tc) {
    tc = tc();
  }
  EXPECT_EQ(ans, (kInput * (kInput + 1)) / 2);
}

} // namespace
