# -*- mode: python; -*-
cc_library(
    name = "concurrent",
    hdrs = [
        "async_event.h",
        "async_parent_task.h",
        "async_queue.h",
        "async_task.h",
        "callback_target.h",
        "error_code.h",
        "latching_signal.h",
        "null_mutex.h",
        "ordered_action_pair.h",
        "ordered_action_sequence.h",
        "parent_startable.h",
        "startable.h",
    ],
    srcs = [
    ],
    copts = ["-std=c++14"],
    deps = [
        "@boost//:headers",
        "@boost//:system",
        "//hittop/util",
        "//hittop/util:functional",
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
        "@boost//:headers",
    ],
)

cc_test(
    name = "async_event-test",
    srcs = [
        "async_event-test.cc",
    ],
    copts = [
        "-Iexternal/gtest/include",
        "-std=c++14",
    ],
    deps = [
        "//hittop",
        ":concurrent",
        "@gtest//:main",
        "@boost//:headers",
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
        "@boost//:headers",
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
        "@boost//:headers",
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
    linkopts = [
        "-Wl,-no_pie",
    ],
    deps = [
        ":concurrent",
        "@gtest//:main",
        "@boost//:headers",
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
        "@boost//:headers",
    ],
)

cc_test(
    name = "async_tasks-test",
    srcs = [
        "async_task-test.cc",
        "async_parent_task-test.cc",
    ],
    copts = [
        "-Iexternal/gtest/include",
        "-std=c++14",
        "-ftemplate-backtrace-limit=0",
    ],
    deps = [
        ":concurrent",
        "@gtest//:main",
        "@boost//:headers",
    ],
)

cc_test(
    name = "startable-test",
    srcs = [
        "startable-test.cc",
    ],
    copts = [
        "-Iexternal/gtest/include",
        "-std=c++14",
        "-ftemplate-backtrace-limit=0",
    ],
    deps = [
        ":concurrent",
        "@gtest//:main",
        "@boost//:headers",
    ],
)
