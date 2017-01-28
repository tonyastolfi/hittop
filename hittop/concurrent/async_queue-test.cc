#include "hittop/concurrent/async_queue.h"
#include "hittop/concurrent/async_queue.h"

#include "gtest/gtest.h"

#include <functional>
#include <memory>
#include <string>

using hittop::concurrent::AsyncQueue;

namespace {
struct TestHandler {
  std::size_t call_count = 0;
  boost::optional<boost::system::error_code> error;
  boost::optional<std::string> value;
  std::unique_ptr<std::string> ptr;

  TestHandler() = default;
  TestHandler(const TestHandler &) = delete;
  TestHandler &operator=(const TestHandler &) = delete;

  void operator()(const boost::system::error_code &ec, std::string *p) {
    ++call_count;
    error = ec;
    if (p) {
      value = std::move(*p);
    }
  }

  void operator()(const boost::system::error_code &ec,
                  std::unique_ptr<std::string> *p) {
    ++call_count;
    error = ec;
    if (p) {
      ptr = std::move(*p);
    } else {
      std::clog << "TestHandler(null) called" << std::endl;
    }
  }
};
} // namespace

class AsyncQueueTest : public ::testing::Test {
protected:
  AsyncQueue<std::string> queue_;
  AsyncQueue<std::unique_ptr<std::string>> ptr_queue_;
  TestHandler handler_;
};

TEST_F(AsyncQueueTest, PushPop) {
  queue_.push("foo");
  auto canceler = queue_.async_pop(std::ref(handler_));
  EXPECT_FALSE(canceler);
  EXPECT_EQ(*handler_.value, "foo");
  EXPECT_FALSE(*handler_.error);
}

TEST_F(AsyncQueueTest, PopPush) {
  auto canceler = queue_.async_pop(std::ref(handler_));
  EXPECT_FALSE(!canceler);
  EXPECT_FALSE(handler_.value);
  EXPECT_FALSE(handler_.error);
  queue_.push("foo");
  EXPECT_EQ(*handler_.value, "foo");
  EXPECT_FALSE(*handler_.error);
}

TEST_F(AsyncQueueTest, EmplacePop) {
  ptr_queue_.emplace(std::make_unique<std::string>("foo"));
  auto canceler = ptr_queue_.async_pop(std::ref(handler_));
  EXPECT_FALSE(canceler);
  EXPECT_EQ(*handler_.ptr, "foo");
  EXPECT_FALSE(*handler_.error);
}

TEST_F(AsyncQueueTest, PopEmplace) {
  EXPECT_EQ(handler_.call_count, 0U);
  auto canceler = ptr_queue_.async_pop(std::ref(handler_));
  EXPECT_FALSE(!canceler);
  EXPECT_FALSE(handler_.ptr);
  EXPECT_FALSE(handler_.error);
  EXPECT_EQ(handler_.call_count, 0U);
  ptr_queue_.emplace(std::make_unique<std::string>("foo"));
  EXPECT_EQ(handler_.call_count, 1U);
  ASSERT_FALSE(!handler_.ptr);
  EXPECT_EQ(*handler_.ptr, "foo");
  EXPECT_FALSE(*handler_.error);
}

TEST_F(AsyncQueueTest, PopPushCancel) {
  auto canceler = queue_.async_pop(std::ref(handler_));
  EXPECT_FALSE(!canceler);
  queue_.push("foo");
  EXPECT_FALSE(canceler());
  EXPECT_EQ(handler_.call_count, 1U);
  EXPECT_FALSE(*handler_.error);
}

TEST_F(AsyncQueueTest, PopCancelPush) {
  auto canceler = queue_.async_pop(std::ref(handler_));
  EXPECT_FALSE(!canceler);
  EXPECT_TRUE(canceler());
  queue_.push("foo");
  EXPECT_EQ(handler_.call_count, 1U);
  EXPECT_EQ(*handler_.error, boost::asio::error::operation_aborted);
}

TEST_F(AsyncQueueTest, PopEmplaceCancel) {
  auto canceler = ptr_queue_.async_pop(std::ref(handler_));
  EXPECT_FALSE(!canceler);
  ptr_queue_.emplace(std::make_unique<std::string>("foo"));
  EXPECT_FALSE(canceler());
  EXPECT_EQ(handler_.call_count, 1U);
  EXPECT_FALSE(*handler_.error);
}

TEST_F(AsyncQueueTest, PopCancelEmplace) {
  auto canceler = ptr_queue_.async_pop(std::ref(handler_));
  EXPECT_FALSE(!canceler);
  EXPECT_TRUE(canceler());
  ptr_queue_.emplace(std::make_unique<std::string>("foo"));
  EXPECT_EQ(handler_.call_count, 1U);
  EXPECT_EQ(*handler_.error, boost::asio::error::operation_aborted);
}
