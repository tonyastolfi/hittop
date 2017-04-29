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
        "const_buffer_sequence.h",
        "const_buffers_handler.h",
        "mutable_buffer_sequence.h",
        "mutable_buffers_handler.h",
        "types.h",
    ],
    copts = ["-std=c++14"],
    deps = [
        "@boost_1_62_0//:headers",
        "@boost_1_62_0//:system",
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
    ],
    copts = [
        "-Iexternal/gtest/include",
        "-std=c++14",
    ],
    deps = [
        "@gtest//:main",
        ":io",
        "//hittop/concurrent"
    ],
)
