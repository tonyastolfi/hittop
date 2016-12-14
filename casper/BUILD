cc_library(
    name = "casper",
    hdrs = [
        "concat.h",
        "literal.h",
        "parser.h",
    ],
    deps = [
        "@boost_1_62_0//:headers",
    ],
    visibility = ["//main:__pkg__"]
)

cc_test(
    name = "concat-test",
    srcs = ["concat-test.cc"],
    copts = ["-Iexternal/gtest/include"],
    deps = [
        "@gtest//:main",
        "//casper:casper",
    ],
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
