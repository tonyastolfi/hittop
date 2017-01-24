#include "hittop/concurrent/callback_target.h"
#include "hittop/concurrent/callback_target.h"

#include "gtest/gtest.h"

#include "boost/optional.hpp"

#include <functional>
#include <future>
#include <string>

/*
  Test plan:

 Callback type: single-shot, listener
 Callback scheduler: dispatch, post
 Function arity: 0, 1, 2, 10

  Verify:

 Function not invoked prematurely
 Function invoked with correct args
 Function invoked on correct strand
 Function invocable with move-only types (unique_ptr)
 Exact number of arg copies made
 Exact number of intrusive_ptr copies made

 */

namespace {

struct CopyCounted {
  int *count = nullptr;

  CopyCounted() = default;
  CopyCounted(const CopyCounted &cc) : count(cc.count) { ++(*count); }
  CopyCounted(CopyCounted &&cc) : count(cc.count) { cc.count = nullptr; }

  CopyCounted &operator=(const CopyCounted &cc) {
    count = cc.count;
    ++(*count);
    return *this;
  }

  CopyCounted &operator=(CopyCounted &&cc) {
    count = cc.count;
    cc.count = nullptr;
    return *this;
  }
};

struct Foo : hittop::concurrent::CallbackTarget<Foo> {
  std::size_t call_count = 0;
  boost::optional<std::size_t> called_with;
  CopyCounted arg0;
  std::unique_ptr<std::string> arg1;
  std::vector<int> argN;

  explicit Foo(boost::asio::io_service &io)
      : hittop::concurrent::CallbackTarget<Foo>(io) {}

  void ZeroArgs() {
    ++call_count;
    called_with = 0;
  }

  void OneArg(CopyCounted a0) {
    ++call_count;
    called_with = 1;
    arg0 = std::move(a0);
  }

  void TwoArgs(CopyCounted a0, std::unique_ptr<std::string> a1) {
    ++call_count;
    called_with = 2;
    arg0 = std::move(a0);
    arg1 = std::move(a1);
  }

  void TenArgs(CopyCounted a0, std::unique_ptr<std::string> a1, int a2, int a3,
               int a4, int a5, int a6, int a7, int a8, int a9) {
    ++call_count;
    called_with = 10;
    arg0 = std::move(a0);
    arg1 = std::move(a1);
    argN = {a2, a3, a4, a5, a6, a7, a8, a9};
  }
};

} // namespace

TEST(CallbackTargetTest, OneShotZeroArgs) {
  boost::asio::io_service io;
  boost::intrusive_ptr<Foo> foo = new Foo(io);

  EXPECT_EQ(foo->use_count(), 1);

  std::function<void()> cb = foo->OneShotCallback(&Foo::ZeroArgs);

  EXPECT_EQ(foo->use_count(), 2);
  EXPECT_EQ(foo->call_count, 0U);
  EXPECT_FALSE(foo->called_with);

  cb();

  EXPECT_EQ(foo->use_count(), 2);
  EXPECT_EQ(foo->call_count, 0U);
  EXPECT_FALSE(foo->called_with);

  io.run_one();
  io.reset();

  EXPECT_EQ(foo->use_count(), 1);
  EXPECT_TRUE(bool{foo->called_with});
  EXPECT_EQ(*foo->called_with, 0UL);
}

TEST(CallbackTargetTest, ListenerZeroArgs) {
  boost::asio::io_service io;
  boost::intrusive_ptr<Foo> foo = new Foo(io);

  EXPECT_EQ(foo->use_count(), 1);

  std::function<void()> cb = foo->ListenerCallback(&Foo::ZeroArgs);

  EXPECT_EQ(foo->use_count(), 2);
  EXPECT_EQ(foo->call_count, 0U);
  EXPECT_FALSE(foo->called_with);

  for (int n = 0; n < 10; ++n) {
    cb();
  }

  EXPECT_EQ(foo->use_count(), 12);
  EXPECT_EQ(foo->call_count, 0U);
  EXPECT_FALSE(foo->called_with);

  io.run();
  io.reset();

  EXPECT_EQ(foo->use_count(), 2);
  EXPECT_EQ(foo->call_count, 10U);
  EXPECT_TRUE(bool{foo->called_with});
  EXPECT_EQ(*foo->called_with, 0U);
}

TEST(CallbackTargetTest, OneShotZeroArgsDispatchNow) {
  boost::asio::io_service io;
  boost::intrusive_ptr<Foo> foo = new Foo(io);
  auto cb = foo->OneShotCallback(&Foo::ZeroArgs);

  EXPECT_FALSE(foo->called_with);

  bool success = false;
  foo->get_strand().post([&cb, foo, &success]() {
    cb();
    success = bool{foo->called_with};
  });

  EXPECT_FALSE(success);

  io.run();
  io.reset();

  EXPECT_TRUE(success);
}

TEST(CallbackTargetTest, OneShotZeroArgsDispatchLater) {
  boost::asio::io_service io;
  boost::intrusive_ptr<Foo> foo = new Foo(io);
  auto cb = foo->OneShotCallback(&Foo::ZeroArgs);

  EXPECT_FALSE(foo->called_with);

  std::promise<void> inside_strand;
  std::promise<void> ok_to_release;

  foo->get_strand().post([&inside_strand, &ok_to_release]() {
    inside_strand.set_value();
    ok_to_release.get_future().get();
  });

  std::thread helper([&io]() { io.run(); });

  bool ran = false;
  bool success = false;
  io.post([&cb, foo, &ran, &success, &inside_strand, &ok_to_release]() {
    ran = true;
    inside_strand.get_future().get();
    cb();
    success = !foo->called_with;
    ok_to_release.set_value();
  });

  EXPECT_FALSE(success);

  io.run();
  helper.join();
  io.reset();

  EXPECT_TRUE(ran);
  EXPECT_TRUE(success);
}

TEST(CallbackTargetTest, OneShotLaterZeroArgs) {
  boost::asio::io_service io;
  boost::intrusive_ptr<Foo> foo = new Foo(io);
  auto cb = foo->OneShotCallbackLater(&Foo::ZeroArgs);

  EXPECT_FALSE(foo->called_with);

  bool success = false;
  foo->get_strand().post([&cb, foo, &success]() {
    cb();
    success = !foo->called_with;
  });

  EXPECT_FALSE(success);
  EXPECT_FALSE(foo->called_with);

  io.run();
  io.reset();

  EXPECT_TRUE(success);
  EXPECT_TRUE(bool{foo->called_with});
}

TEST(CallbackTargetTest, OneShotZeroArgsPost) {}

TEST(CallbackTargetTest, Lambdas) {
  std::function<std::string()> f;
  {
    std::string s = "hello";
    std::string &ref = s;
    auto &&rref = std::move(ref);
    f = [t = std::forward<std::string &>(rref)]() { return t; };
  }
  EXPECT_EQ(f(), "hello");
}
