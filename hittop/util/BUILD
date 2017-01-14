cc_library(
    name = "util",
    hdrs = [
        "fallible.h",
        "scope_exit.h",
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
