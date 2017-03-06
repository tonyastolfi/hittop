cc_library(
    name = "json",
    hdrs = [
        "grammar.h",
        "parse_visitor.h",
        "parser.h",
        "types.h",
    ],
    srcs = [
    ],
    copts = ["-std=c++14"],
    deps = [
        "@boost_1_62_0//:headers",
        "//hittop/parser",
    ],
    visibility = ["//visibility:public"]
)

cc_test(
    name = "json-test",
    srcs = [
        "parser-test.cc",
        "types-test.cc",
    ],
    data = [
        ":test-data.json"
    ],
    copts = [
        "-Iexternal/gtest/include",
        "-std=c++14",
    ],
    deps = [
        "@gtest//:main",
        ":json",
        "//hittop/util:test_util"
    ],
)
