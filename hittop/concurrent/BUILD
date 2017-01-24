cc_library(
    name = "concurrent",
    hdrs = [
        "async_queue.h",
        "callback_target.h",
        "latch.h",
    ],
    srcs = [
    ],
    copts = ["-std=c++14"],
    deps = [
        "@boost_1_62_0//:headers",
        "@boost_1_62_0//:system",
        "//hittop/util",
        "//hittop/util:tuples",
    ],
    visibility = ["//visibility:public"]
)

cc_test(
    name = "callback_target-test",
    srcs = [
        "async_queue-test.cc",
        "callback_target-test.cc",
        "latch-test.cc",
    ],
    copts = [
        "-Iexternal/gtest/include",
        "-std=c++14",
    ],
    deps = [
        ":concurrent",
        "@gtest//:main",
        "@boost_1_62_0//:headers",
    ],
)

cc_test(
    name = "async_queue-test",
    srcs = [
        "async_queue-test.cc",
    ],
    copts = [
        "-Iexternal/gtest/include",
        "-std=c++14",
    ],
    deps = [
        ":concurrent",
        "@gtest//:main",
        "@boost_1_62_0//:headers",
    ],
)

cc_test(
    name = "latch-test",
    srcs = [
        "latch-test.cc",
    ],
    copts = [
        "-Iexternal/gtest/include",
        "-std=c++14",
    ],
    deps = [
        ":concurrent",
        "@gtest//:main",
        "@boost_1_62_0//:headers",
    ],
)
