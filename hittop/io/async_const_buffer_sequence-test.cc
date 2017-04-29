#include "hittop/io/async_const_buffer_sequence.h"
#include "hittop/io/async_const_buffer_sequence.h"

#include "gtest/gtest.h"

#include <iostream>
#include <vector>

#include "boost/asio/buffer.hpp"
#include "boost/asio/buffers_iterator.hpp"

namespace {

using hittop::io::AsyncConstBufferSequence;
using ConstBuffers = std::vector<hittop::io::const_buffer>;
using hittop::io::error_code;

class AsyncConstBufferSequenceTest : public ::testing::Test {
protected:
  AsyncConstBufferSequenceTest() : buffers_(parts_.size()) {
    std::transform(
        parts_.begin(), parts_.end(), buffers_.begin(),
        [](const std::string &part) { return boost::asio::buffer(part); });

    all_data_.assign(boost::asio::buffers_begin(buffers_),
                     boost::asio::buffers_end(buffers_));
  }

  std::vector<std::string> parts_{"two atoms may\n",
                                  "our theories say\n",
                                  "reside inside a common place\n",
                                  "if they are very nearly still\n",
                                  "so what is space\n",
                                  "but blood flush red behind the face?\n"};

  ConstBuffers buffers_;
  std::string all_data_;
};

TEST_F(AsyncConstBufferSequenceTest, FetchInChunks) {
  for (std::size_t chunk_size = 1; chunk_size <= all_data_.size();
       chunk_size += 3) {
    AsyncConstBufferSequence<ConstBuffers> s(buffers_);
    std::string unread = all_data_;
    while (!unread.empty()) {
      error_code error;
      std::string fetched;
      EXPECT_EQ(s.size(), unread.size());
      EXPECT_EQ(s.max_size(), unread.size());
      s.async_fetch(1, [&](const error_code &ec, const auto &bs) {
        error = ec;
        fetched.assign(boost::asio::buffers_begin(bs),
                       boost::asio::buffers_end(bs));
      });
      EXPECT_TRUE(!error);
      EXPECT_EQ(unread, fetched);
      const std::size_t bytes_to_consume = std::min(chunk_size, unread.size());
      unread.erase(0, bytes_to_consume);
      s.consume(bytes_to_consume);
    }
    EXPECT_EQ(s.size(), unread.size());
    EXPECT_EQ(s.max_size(), unread.size());

    error_code error;
    std::size_t fetched_size;
    s.async_fetch(1, [&](const error_code &ec, const auto &bs) {
      error = ec;
      fetched_size = boost::asio::buffer_size(bs);
    });
    EXPECT_EQ(fetched_size, 0U);
    EXPECT_EQ(error, boost::asio::error::eof);
  }
}

TEST_F(AsyncConstBufferSequenceTest, FetchTooMuch) {
  AsyncConstBufferSequence<ConstBuffers> s(buffers_);
  for (std::size_t min_size = 1; min_size <= all_data_.size(); ++min_size) {
    error_code error;
    std::size_t fetched_size;
    s.async_fetch(1, [&](const error_code &ec, const auto &bs) {
      error = ec;
      fetched_size = boost::asio::buffer_size(bs);
    });
    EXPECT_TRUE(!error);
    EXPECT_EQ(fetched_size, all_data_.size());
    s.consume(0);
  }
  error_code error;
  std::size_t fetched_size;
  s.async_fetch(all_data_.size() + 1,
                [&](const error_code &ec, const auto &bs) {
                  error = ec;
                  fetched_size = boost::asio::buffer_size(bs);
                });
  EXPECT_EQ(fetched_size, 0U);
  EXPECT_EQ(error, boost::asio::error::eof);
}

} // namespace
