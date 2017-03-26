#include "hittop/io/async_circular_buffer_stream.h"
#include "hittop/io/async_circular_buffer_stream.h"

#include "gtest/gtest.h"

namespace {

using ::hittop::io::AsyncCircularBufferStream;

class AsyncCircularBufferStreamTest : public ::testing::Test {
protected:
  AsyncCircularBufferStream buffer_{3}; // 2^3 == 8 byte capcacity
};

TEST_F(AsyncCircularBufferStreamTest, CtorDtor) {}

} // namespace
