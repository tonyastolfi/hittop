cc_library(
    name = "json",
    hdrs = [
        "grammar.h",
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
        "types-test.cc",
    ],
    copts = [
        "-Iexternal/gtest/include",
        "-std=c++14",
    ],
    deps = [
        "@gtest//:main",
        ":json",
    ],
)
