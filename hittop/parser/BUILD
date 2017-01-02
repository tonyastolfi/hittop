cc_library(
    name = "parser",
    hdrs = [
        "concat.h",
        "literal.h",
        "parse_error.h",
        "parser.h",
        "repeat.h",
    ],
    copts = ["-std=c++14"],
    deps = [
        "@boost_1_62_0//:headers",
    ],
    visibility = ["//visibility:public"]
)

cc_test(
    name = "parser-test",
    srcs = [
        "concat-test.cc",
        "literal-test.cc",
        "parse_error-test.cc",
        "repeat-test.cc",
    ],
    copts = [
        "-Iexternal/gtest/include",
        "-std=c++14",
    ],
    deps = [
        "@gtest//:main",
        ":parser",
        "//hittop/util",
    ],
)
