cc_library(
    name = "util",
    hdrs = [
        "fallible.h",
        "load_file_as_string.h",
        "range_to_string.h",
        "scope_exit.h",
        "type_traits.h",
    ],
    srcs = [
        "load_file_as_string.cc",
    ],
    copts = ["-std=c++14"],
    deps = [
        "@boost_1_62_0//:headers",
    ],
    visibility = ["//visibility:public"]
)

cc_test(
    name = "util-test",
    srcs = [
        "fallible-test.cc",
    ],
    copts = [
        "-Iexternal/gtest/include",
        "-std=c++14",
    ],
    deps = [
        "@gtest//:main",
        ":util",
        "//hittop/parser",
    ],
)

cc_library(
    name = "test_util",
    hdrs = [
        "test_data.h",
    ],
    srcs = [
        "test_data.cc",
    ],
    copts = [
        "-Iexternal/gtest/include",
        "-std=c++14"
    ],
    deps = [
        ":util",
        "@gtest//:main",
        "@boost_1_62_0//:headers",
    ],
    visibility = ["//visibility:public"]
)

cc_library(
    name = "tuples",
    hdrs = [
        "tuples.h",
    ],
    copts = [
        "-std=c++14"
    ],
    deps = [
        "@boost_1_62_0//:headers",
    ],
    visibility = ["//visibility:public"]
)

cc_test(
    name = "tuples-test",
    srcs = [
        "tuples-test.cc",
    ],
    copts = [
        "-Iexternal/gtest/include",
        "-std=c++14",
    ],
    deps = [
        "@gtest//:main",
        ":tuples",
    ],
)

cc_library(
    name = "functional",
    hdrs = [
        "first_match.h",
        "is_callable.h",
    ],
    copts = [
        "-std=c++14"
    ],
    deps = [
        ":tuples",
        "@boost_1_62_0//:headers",
    ],
    visibility = ["//visibility:public"]
)

cc_test(
    name = "functional-test",
    srcs = [
        "is_callable-test.cc",
    ],
    copts = [
        "-Iexternal/gtest/include",
        "-std=c++14",
    ],
    deps = [
        "@gtest//:main",
        ":functional",
    ],
)

cc_library(
    name = "abi",
    hdrs = [
        "demangle.h",
    ],
    copts = [
        "-std=c++14"
    ],
    deps = [
        "@boost_1_62_0//:headers",
    ],
    visibility = ["//visibility:public"]
)

cc_library(
    name = "debug",
    hdrs = [
        "backtrace.h",
    ],
    copts = [
        "-std=c++14"
    ],
    deps = [
    ],
    visibility = ["//visibility:public"]
)

cc_library(
    name="in_place_factory",
    hdrs = [
        "in_place_alloc_factory.h",
        "saved_in_place_factory.h",
    ],
    srcs = [
    ],
    copts = ["-std=c++14"],
    deps = [
        "@boost_1_62_0//:headers",
        ":tuples",
    ],
    visibility = ["//visibility:public"]
)

cc_test(
    name = "in_place_factory-test",
    srcs = [
        "in_place_alloc_factory-test.cc",
        "saved_in_place_factory-test.cc",
    ],
    copts = [
        "-Iexternal/gtest/include",
        "-std=c++14",
    ],
    deps = [
        "@gtest//:main",
        ":in_place_factory",
    ],
)
