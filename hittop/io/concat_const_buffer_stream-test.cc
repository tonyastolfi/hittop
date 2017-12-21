#include "hittop/io/concat_const_buffer_stream.h"
#include "hittop/io/concat_const_buffer_stream.h"

#include "gtest/gtest.h"

#include "hittop/concurrent/latching_signal.h"
#include "hittop/io/empty_const_buffer_stream.h"

namespace {

using hittop::concurrent::LatchingSignal;
namespace io = hittop::io;

// Test plan:
//  - Exhaustive, randomized:
//  -- High level cases: {empty, bytes}, {bytes, empty}, {bytes, bytes}
//  -- For each fetch, honor min_count, but flip a coin as to whether more data
//  (and how much) arrived since the last fetch, up to the EOS
//  -- 20% chance of error for any fetch
//  -- On successful fetches, break data into random sized buffer chunks each
//  time.
//  -- consume a random amount in the test driver after each successful fetch
//  -- verify data arrives correctly, in-order
//  -- verify min_count always honored
//  -- verify fetch always returns min of max_size and actual bytes available
//  - Hand-coded cases:
//  -- exceed max_size()
//  -- fetch zero bytes
//  -- close_for_read() passes through to both
//  -- concurrent fetch from first and second:
//  --- first completes before second
//  --- second completes before first
//  --- real thread stress test

TEST(ConcatConstBufferStreamTest, DefaultCtor) {
  io::ConcatConstBufferStream<io::EmptyConstBufferStream,
                              io::EmptyConstBufferStream>
      s;

  using ConstBuffers = decltype(s)::const_buffers_type;
  using MockFetchHandler =
      LatchingSignal<void(const io::error_code &, const ConstBuffers &)>;

  {
    MockFetchHandler handler;
    s.async_fetch(1, std::ref(handler));
    ASSERT_TRUE(handler.is_latched());
    EXPECT_EQ(io::error_code{boost::asio::error::eof}, *handler.arg<0>());
  }
}

} // anonymous namespace
