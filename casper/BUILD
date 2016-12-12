cc_library(
    name = "casper",
    hdrs = [
        "literal.h",
        "parser.h",
    ],
    visibility = ["//main:__pkg__"]
)

cc_test(
    name = "literal-test",
    srcs = ["literal-test.cc"],
    copts = ["-Iexternal/gtest/include"],
    deps = [
        "@gtest//:main",
        "//casper:casper",
    ],
)

cc_test(
    name = "parser-test",
    srcs = ["parser-test.cc"],
    copts = ["-Iexternal/gtest/include"],
    deps = [
        "@gtest//:main",
        "//casper:casper",
    ],
)
