#include "hittop/concurrent/ordered_action_sequence.h"
#include "hittop/concurrent/ordered_action_sequence.h"

#include "gtest/gtest.h"

#include <algorithm>
#include <cstddef>
#include <thread>
#include <vector>

namespace {

const std::size_t kNumIterations = 1000;
const std::size_t kNumThreads = 500;
const std::size_t kNumInserts = 1000;

using ::hittop::concurrent::OrderedActionSequence;

TEST(OrderedActionSequenceTest, MultiThreadTest) {
  for (size_t n = 0; n < kNumIterations; ++n) {
    std::vector<int> v;
    std::vector<std::function<void()>> actions;
    OrderedActionSequence sequence;
    for (std::size_t i = 0; i < kNumInserts; ++i) {
      actions.push_back(sequence.WrapNext([&v, i]() { v.push_back(i); }));
    }
    std::random_shuffle(actions.begin(), actions.end());
    std::vector<std::thread> threads;
    std::size_t next_insert = 0;
    for (std::size_t i = 0; i < kNumThreads; ++i) {
      const std::size_t inserts_left = kNumInserts - next_insert;
      const std::size_t threads_left = kNumThreads - i;
      const std::size_t inserts_this_thread = inserts_left / threads_left;
      threads.emplace_back([&actions, next_insert, inserts_this_thread]() {
        for (std::size_t j = next_insert; j < next_insert + inserts_this_thread;
             ++j) {
          actions[j]();
        }
      });
      next_insert += inserts_this_thread;
    }
    for (auto &t : threads) {
      t.join();
    }
    ASSERT_EQ(v.size(), kNumInserts);
    for (std::size_t i = 0; i < kNumInserts; ++i) {
      EXPECT_EQ(v[i], i);
    }
  }
}

} // namespace
