cc_library(
    name = "concurrent",
    hdrs = [
        "async_queue.h",
        "callback_target.h",
        "latching_signal.h",
        "null_mutex.h",
        "ordered_action_pair.h",
        "ordered_action_sequence.h",
        "tail_call.h",
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
        "callback_target-test.cc",
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
    name = "latching_signal-test",
    srcs = [
        "latching_signal-test.cc",
    ],
    copts = [
        "-Iexternal/gtest/include",
        "-std=c++14",
        "-ftemplate-backtrace-limit=0",
    ],
    deps = [
        ":concurrent",
        "@gtest//:main",
        "@boost_1_62_0//:headers",
    ],
)

cc_test(
    name = "ordered_actions-test",
    srcs = [
        "ordered_action_pair-test.cc",
        "ordered_action_sequence-test.cc",
    ],
    copts = [
        "-Iexternal/gtest/include",
        "-std=c++14",
        "-ftemplate-backtrace-limit=0",
    ],
    deps = [
        ":concurrent",
        "@gtest//:main",
        "@boost_1_62_0//:headers",
    ],
)

cc_test(
    name = "null_mutex-test",
    srcs = [
        "null_mutex-test.cc",
    ],
    copts = [
        "-Iexternal/gtest/include",
        "-std=c++14",
        "-ftemplate-backtrace-limit=0",
    ],
    deps = [
        ":concurrent",
        "@gtest//:main",
        "@boost_1_62_0//:headers",
    ],
)
