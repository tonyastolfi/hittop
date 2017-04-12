cc_library(
    name = "uri",
    hdrs = [
        "basic_uri.h",
        "grammar.h",
        "mutable_path_segments.h",
        "mutable_uri.h",
        "uri_parse_visitor.h"
    ],
    copts = ["-std=c++14"],
    deps = [
        "//hittop/util:util",
        "//hittop/util:functional",
        "//hittop/util:in_place_factory",
        "//hittop/parser",
        "//third_party/short_alloc",
        "@boost_1_62_0//:headers",
    ],
    visibility = ["//visibility:public"]
)

cc_test(
    name = "grammar-test",
    srcs = [
        "grammar-test.cc",
    ],
    copts = [
        "-Iexternal/gtest/include",
        "-std=c++14",
    ],
    deps = [
        "@gtest//:main",
        ":uri",
        "//hittop/parser",
        "//hittop/util",
        "//hittop/util:functional",
    ],
)

cc_test(
    name = "basic_uri-test",
    srcs = [
        "basic_uri-test.cc",
    ],
    copts = [
        "-Iexternal/gtest/include",
        "-std=c++14",
    ],
    deps = [
        "@gtest//:main",
        ":uri",
        "//hittop/parser",
        "//hittop/util",
        "//hittop/util:functional",
    ],
)

cc_test(
    name = "parse-test",
    srcs = [
        "parse-test.cc",
    ],
    copts = [
        "-Iexternal/gtest/include",
        "-std=c++14",
    ],
    deps = [
        "@gtest//:main",
        ":uri",
        "//hittop/parser",
        "//hittop/util",
        "//hittop/util:functional",
    ],
)
