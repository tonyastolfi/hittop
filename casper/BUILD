cc_library(
    name = "casper",
    hdrs = [
        "concat.h",
        "fallible.h",
        "literal.h",
        "parser.h",
    ],
    copts = ["-std=c++14"],
    deps = [
        "@boost_1_62_0//:headers",
    ],
    visibility = ["//main:__pkg__"]
)

cc_test(
    name = "casper-test",
    srcs = [
        "concat-test.cc",
        "fallible-test.cc",
        "literal-test.cc",
        "parser-test.cc"
    ],
    copts = [
        "-Iexternal/gtest/include",
        "-std=c++14",
    ],
    deps = [
        "@gtest//:main",
        "//casper:casper",
    ],
)
