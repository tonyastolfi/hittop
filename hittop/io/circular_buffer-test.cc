#include "hittop/io/circular_buffer.h"
#include "hittop/io/circular_buffer.h"

#include "gtest/gtest.h"

// Test plan:
//
//  1. Verify initial state
//    a-?. for different buffer sizes from 1 to 4096.
//  2. Verify not copyable
//  3. Two-threaded stress test
//  4. read before write
//  5. write before read
//  6. don't consume all (0, >0), read again
//  7. don't commit all (0, >0), write again
//  8. multiple reads before write
//  9. multiple writes before read
//  10. debug dump
//
