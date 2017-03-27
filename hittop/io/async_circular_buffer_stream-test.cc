#include "hittop/io/async_circular_buffer_stream.h"
#include "hittop/io/async_circular_buffer_stream.h"

#include "gtest/gtest.h"

#include "boost/asio/buffers_iterator.hpp"

#include "hittop/concurrent/latching_signal.h"

/*
Test plan:

1. write then read
2. read then write
3. write then read, not enough, reader waits for second write
4. write then read, then write, not enough, writer waits for read
5. 3, but read then write first
6. 4, but read then write first
7. concurrent write, error
8. write after close for write, error
9. write after close for read, error
10. prepare larger than buffer size, error
11. concurrent read, error
12. read after close for write, drain buffer until eof
13. 12, but request more than available; fail
14. fetch larger than buffer size, error
15. close for write unblocks writer
16. close for write unblocks reader
17. close for read unblocks reader
18. close for read unblocks writer
 */
namespace {

using ::hittop::io::AsyncCircularBufferStream;
using ::hittop::concurrent::LatchingSignal;
using ::hittop::io::error_code;

using MockFetchHandler = LatchingSignal<void(
    const error_code &, AsyncCircularBufferStream::const_buffers_type)>;
using MockPrepareHandler = LatchingSignal<void(
    const error_code &, AsyncCircularBufferStream::mutable_buffers_type)>;

class AsyncCircularBufferStreamTest : public ::testing::Test {
protected:
  void Prepare(std::size_t minimum = 1) {
    buffer_->async_prepare(minimum, std::ref(handle_prepare_));
  }

  bool IsPrepareComplete() const { return handle_prepare_.is_latched(); }

  error_code PrepareError() const { return *handle_prepare_.arg<0>(); }

  std::size_t BufferSize() const {
    return boost::asio::buffer_size(*handle_prepare_.arg<1>());
  }

  template <typename Range> void WriteAndCommit(const Range &bytes) {
    std::size_t size = std::distance(std::begin(bytes), std::end(bytes));
    std::copy(std::begin(bytes), std::end(bytes),
              boost::asio::buffers_begin(*handle_prepare_.arg<1>()));
    buffer_->commit(size);
    handle_prepare_.reset();
  }

  void Fetch(std::size_t minimum = 1) {
    buffer_->async_fetch(minimum, std::ref(handle_fetch_));
  }

  bool IsFetchComplete() const { return handle_fetch_.is_latched(); }

  error_code FetchError() const { return *handle_fetch_.arg<0>(); }

  std::size_t DataSize() const {
    return boost::asio::buffer_size(*handle_fetch_.arg<1>());
  }

  std::string Data() const {
    auto &fetched_buffers = *handle_fetch_.arg<1>();
    return std::string(boost::asio::buffers_begin(fetched_buffers),
                       boost::asio::buffers_end(fetched_buffers));
  }

  void Consume(std::size_t count) {
    buffer_->consume(count);
    handle_fetch_.reset();
  }

  void DeleteBuffer() { buffer_.reset(); }

  std::string test_data(std::size_t size = 6) const {
    static const std::string s("1234567890");
    return s.substr(0, size);
  }

  MockPrepareHandler handle_prepare_;
  MockFetchHandler handle_fetch_;
  std::unique_ptr<AsyncCircularBufferStream> buffer_{
      new AsyncCircularBufferStream(3)}; // 2^3 == 8 byte capcacity
};

TEST_F(AsyncCircularBufferStreamTest, CtorDtor) {}

// 1. write then read
TEST_F(AsyncCircularBufferStreamTest, WriteCommitRead) {
  Prepare();

  EXPECT_TRUE(IsPrepareComplete());
  EXPECT_EQ(PrepareError(), error_code{});
  EXPECT_EQ(BufferSize(), 8U);

  WriteAndCommit(test_data());
  Fetch();

  EXPECT_TRUE(IsFetchComplete());
  EXPECT_EQ(FetchError(), error_code{});
  EXPECT_EQ(test_data(), Data());
}

TEST_F(AsyncCircularBufferStreamTest, WriteReadCommit) {
  Prepare();
  Fetch();

  EXPECT_TRUE(IsPrepareComplete());
  EXPECT_FALSE(IsFetchComplete());
  EXPECT_EQ(PrepareError(), error_code{});
  EXPECT_EQ(BufferSize(), 8U);

  WriteAndCommit(test_data());

  EXPECT_TRUE(IsFetchComplete());
  EXPECT_EQ(FetchError(), error_code{});
  EXPECT_EQ(test_data(), Data());
}

// 2. read then write
TEST_F(AsyncCircularBufferStreamTest, ReadWrite) {
  Fetch();
  Prepare();

  EXPECT_TRUE(IsPrepareComplete());
  EXPECT_FALSE(IsFetchComplete());
  EXPECT_EQ(PrepareError(), error_code{});
  EXPECT_EQ(BufferSize(), 8U);

  WriteAndCommit(test_data());

  EXPECT_TRUE(IsFetchComplete());
  EXPECT_EQ(FetchError(), error_code{});
  EXPECT_EQ(test_data(), Data());
}

// 3. write then read, not enough, reader waits for second write
TEST_F(AsyncCircularBufferStreamTest, WriteReadDelayedWriterUnblocks) {
  Prepare();
  WriteAndCommit(test_data(1));
  Fetch(2);

  EXPECT_FALSE(IsFetchComplete());

  Prepare();
  WriteAndCommit(test_data(2));

  EXPECT_TRUE(IsFetchComplete());
  EXPECT_EQ(FetchError(), error_code{});
  EXPECT_EQ(Data(), test_data(1) + test_data(2));
}

// 4. write then read, then write, not enough, writer waits for read
TEST_F(AsyncCircularBufferStreamTest, WriteReadWriteDelayedReaderUnblocks) {
  Prepare(8);
  WriteAndCommit(test_data(8));
  Fetch(4);

  EXPECT_TRUE(IsFetchComplete());

  Prepare(3);

  EXPECT_FALSE(IsPrepareComplete());

  Consume(1);

  EXPECT_FALSE(IsPrepareComplete());

  Fetch();
  Consume(2);

  EXPECT_TRUE(IsPrepareComplete());
}

// 5. 3, but read then write first
TEST_F(AsyncCircularBufferStreamTest, ReadWriteReadDelayedWriterUnblocks) {
  Fetch(2);
  Prepare();
  WriteAndCommit(test_data(1));

  EXPECT_FALSE(IsFetchComplete());

  Prepare();
  WriteAndCommit(test_data(2));

  EXPECT_TRUE(IsFetchComplete());
  EXPECT_EQ(FetchError(), error_code{});
  EXPECT_EQ(Data(), test_data(1) + test_data(2));
}

// 6. 4, but read then write first
TEST_F(AsyncCircularBufferStreamTest, ReadWriteWriteDelayedReaderUnblocks) {
  Fetch(4);
  Prepare(8);
  WriteAndCommit(test_data(8));

  EXPECT_TRUE(IsFetchComplete());

  Prepare(3);

  EXPECT_FALSE(IsPrepareComplete());

  Consume(1);

  EXPECT_FALSE(IsPrepareComplete());

  Fetch();
  Consume(2);

  EXPECT_TRUE(IsPrepareComplete());
}

// 7. concurrent write, error
TEST_F(AsyncCircularBufferStreamTest, ConcurrentWrites1) {
  Prepare();

  EXPECT_TRUE(IsPrepareComplete());

  MockPrepareHandler handle_error;
  buffer_->async_prepare(1, std::ref(handle_error));

  EXPECT_TRUE(handle_error.is_latched());
  EXPECT_EQ(*handle_error.arg<0>(),
            error_code{boost::asio::error::already_started});
}

TEST_F(AsyncCircularBufferStreamTest, ConcurrentWrites2) {
  Prepare(8);
  WriteAndCommit(test_data(8));

  Prepare();

  EXPECT_FALSE(IsPrepareComplete());

  MockPrepareHandler handle_error;
  buffer_->async_prepare(1, std::ref(handle_error));

  EXPECT_TRUE(handle_error.is_latched());
  EXPECT_EQ(*handle_error.arg<0>(),
            error_code{boost::asio::error::already_started});
}

// 8. write after close for write, error
TEST_F(AsyncCircularBufferStreamTest, WriteAfterCloseForWrite) {
  buffer_->close_for_write();

  MockPrepareHandler handle_error;
  buffer_->async_prepare(1, std::ref(handle_error));

  EXPECT_TRUE(handle_error.is_latched());
  EXPECT_EQ(*handle_error.arg<0>(), error_code{boost::asio::error::shut_down});
}

// 9. write after close for read, error
TEST_F(AsyncCircularBufferStreamTest, WriteAfterCloseForRead) {
  buffer_->close_for_read();

  MockPrepareHandler handle_error;
  buffer_->async_prepare(1, std::ref(handle_error));

  EXPECT_TRUE(handle_error.is_latched());
  EXPECT_EQ(*handle_error.arg<0>(),
            error_code{boost::asio::error::broken_pipe});
}

// 10. prepare larger than buffer size, error
TEST_F(AsyncCircularBufferStreamTest, WriteTooLarge) {
  for (int n = 0; n < 2; ++n) {
    Prepare(9);

    EXPECT_TRUE(IsPrepareComplete());
    EXPECT_EQ(PrepareError(), error_code{boost::asio::error::invalid_argument});
  }
}

// 11. concurrent read, error
TEST_F(AsyncCircularBufferStreamTest, ConcurrentRead1) {
  Fetch();

  EXPECT_FALSE(IsFetchComplete());

  MockFetchHandler handle_error;
  buffer_->async_fetch(1, std::ref(handle_error));

  EXPECT_TRUE(handle_error.is_latched());
  EXPECT_EQ(*handle_error.arg<0>(),
            error_code{boost::asio::error::already_started});
}

TEST_F(AsyncCircularBufferStreamTest, ConcurrentRead2) {
  Prepare();
  WriteAndCommit(test_data(1));
  Fetch(1);

  EXPECT_TRUE(IsFetchComplete());

  MockFetchHandler handle_error;
  buffer_->async_fetch(1, std::ref(handle_error));

  EXPECT_TRUE(handle_error.is_latched());
  EXPECT_EQ(*handle_error.arg<0>(),
            error_code{boost::asio::error::already_started});
}

// 12. read after close for write, drain buffer until eof
TEST_F(AsyncCircularBufferStreamTest, WriteCloseForWriteReadUntilEof) {
  const auto data = test_data(8);

  Prepare(8);
  WriteAndCommit(data);
  buffer_->close_for_write();

  for (int i = 0; i < 8; ++i) {
    Fetch(1);
    EXPECT_TRUE(IsFetchComplete());
    EXPECT_EQ(FetchError(), error_code{});
    EXPECT_EQ(Data(), data.substr(i));
    Consume(1);
  }

  Fetch(1);
  EXPECT_TRUE(IsFetchComplete());
  EXPECT_EQ(FetchError(), error_code{boost::asio::error::eof});
}

// 13. 12, but request more than available; fail
TEST_F(AsyncCircularBufferStreamTest, WriteCloseForWriteReadMoreThanAvailable) {
  Prepare(4);
  WriteAndCommit(test_data(4));
  buffer_->close_for_write();
  Fetch(5);

  EXPECT_TRUE(IsFetchComplete());
  EXPECT_EQ(FetchError(), error_code{boost::asio::error::eof});

  Fetch(6);

  EXPECT_TRUE(IsFetchComplete());
  EXPECT_EQ(FetchError(), error_code{boost::asio::error::eof});
}

// 14. fetch larger than buffer size, error
TEST_F(AsyncCircularBufferStreamTest, ReadTooLarge) {
  for (int n = 0; n < 2; ++n) {
    Fetch(9);

    EXPECT_TRUE(IsFetchComplete());
    EXPECT_EQ(FetchError(), error_code{boost::asio::error::invalid_argument});
  }
}

// 15. close for write unblocks writer
TEST_F(AsyncCircularBufferStreamTest, CloseForWriteUnblocksWriter) {
  Prepare(8);
  WriteAndCommit(test_data(8));
  Prepare();

  EXPECT_FALSE(IsPrepareComplete());

  buffer_->close_for_write();

  EXPECT_TRUE(IsPrepareComplete());
  EXPECT_EQ(PrepareError(), error_code{boost::asio::error::shut_down});
}

// 16. close for write unblocks reader
TEST_F(AsyncCircularBufferStreamTest, CloseForWriteUnblocksReader) {
  Fetch();

  EXPECT_FALSE(IsFetchComplete());

  buffer_->close_for_write();

  EXPECT_TRUE(IsFetchComplete());
  EXPECT_EQ(FetchError(), error_code{boost::asio::error::eof});
}

// 17. close for read unblocks reader
TEST_F(AsyncCircularBufferStreamTest, CloseForReadUnblocksReader) {
  Fetch();

  EXPECT_FALSE(IsFetchComplete());

  buffer_->close_for_read();

  EXPECT_TRUE(IsFetchComplete());
  EXPECT_EQ(FetchError(), error_code{boost::asio::error::shut_down});
}

// 18. close for read unblocks writer
TEST_F(AsyncCircularBufferStreamTest, CloseForReadUnblocksWriter) {
  Prepare(8);
  WriteAndCommit(test_data(8));
  Prepare();

  EXPECT_FALSE(IsPrepareComplete());

  buffer_->close_for_read();

  EXPECT_TRUE(IsPrepareComplete());
  EXPECT_EQ(PrepareError(), error_code{boost::asio::error::broken_pipe});
}

// 19. close for read unblocks reader and writer
TEST_F(AsyncCircularBufferStreamTest, CloseForReadUnblocksReaderAndWriter) {
  Fetch(5);
  Prepare(4);
  WriteAndCommit(test_data(4));
  Prepare(5);

  EXPECT_FALSE(IsFetchComplete());
  EXPECT_FALSE(IsPrepareComplete());

  buffer_->close_for_read();

  EXPECT_TRUE(IsFetchComplete());
  EXPECT_TRUE(IsPrepareComplete());
  EXPECT_EQ(FetchError(), error_code{boost::asio::error::shut_down});
  EXPECT_EQ(PrepareError(), error_code{boost::asio::error::broken_pipe});
}

// 20. close for write unblocks reader and writer
TEST_F(AsyncCircularBufferStreamTest, CloseForWriteUnblocksReaderAndWriter) {
  Fetch(5);
  Prepare(4);
  WriteAndCommit(test_data(4));
  Prepare(5);

  EXPECT_FALSE(IsFetchComplete());
  EXPECT_FALSE(IsPrepareComplete());

  buffer_->close_for_write();

  EXPECT_TRUE(IsFetchComplete());
  EXPECT_TRUE(IsPrepareComplete());
  EXPECT_EQ(FetchError(), error_code{boost::asio::error::eof});
  EXPECT_EQ(PrepareError(), error_code{boost::asio::error::shut_down});
}

} // namespace
