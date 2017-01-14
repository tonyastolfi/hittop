cc_library(
    name = "util",
    hdrs = [
        "fallible.h",
        "load_file_as_string.h",
        "scope_exit.h",
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
    copts = ["-std=c++14"],
    deps = [
        ":util",
        "@gtest//:main",
        "@boost_1_62_0//:headers",
    ],
    visibility = ["//visibility:public"]
)
