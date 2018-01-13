# -*- mode: python; -*-
cc_library(
    name = "io",
    hdrs = [
        "async_circular_buffer_stream.h",
        "async_const_buffer_sequence.h",
        "async_const_buffer_stream.h",
        "async_mutable_buffer_stream.h",
        "async_transfer_task.h",
        "async_reader.h",
        "async_writer.h",
        "basic_buffer_sequence.h",
        "circular_buffer.h",
        "concat_const_buffer_stream.h",
        "const_buffer_sequence.h",
        "const_buffers_handler.h",
        "empty_const_buffer_stream.h",
        "generic_const_buffer_stream.h",
        "generic_server.h",
        "mutable_buffer_sequence.h",
        "mutable_buffers_handler.h",
        "types.h",
    ],
    copts = ["-std=c++1z"],
    deps = [
        "@boost//:headers",
        "@boost//:system",
    ],
    visibility = ["//visibility:public"]
)

cc_test(
    name = "io-test",
    srcs = [
        "async_circular_buffer_stream-test.cc",
        "async_const_buffer_sequence-test.cc",
        "async_reader-test.cc",
        "async_writer-test.cc",
        "circular_buffer-test.cc",
        "concat_const_buffer_stream-test.cc",
        "empty_const_buffer_stream-test.cc",
    ],
    copts = [
        "-Iexternal/gtest/include",
        "-std=c++1z",
    ],
    deps = [
        "@gtest//:main",
        ":io",
        "//hittop/concurrent"
    ],
)
