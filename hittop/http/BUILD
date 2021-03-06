cc_library(
    name = "http",
    hdrs = [
        "basic_header.h",
        "basic_request.h",
        "grammar.h",
        "parse_request.h",
        "request.h",
        "request_parse_visitor.h",
    ],
    copts = ["-std=c++14"],
    deps = [
        "//hittop/util:util",
        "//hittop/util:functional",
        "//hittop/uri",
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
        ":http",
        "//hittop/parser",
        "//hittop/util",
    ],
)

cc_test(
    name = "parse-test",
    srcs = [
        "parse-test.cc",
        "parse_request-test.cc",
    ],
    data = [
        ":chrome_request.bin",
        ":chrome_request2.bin",
        ":curl_request.bin",
        ":google_response.bin",
    ],
    copts = [
        "-Iexternal/gtest/include",
        "-std=c++14",
    ],
    deps = [
        "@gtest//:main",
        ":http",
        "//hittop/parser",
        "//hittop/util",
        "//hittop/util:functional",
        "//hittop/util:test_util",
    ],
)

cc_binary(
    name = "parse_request_bench",
    srcs = [
        "parse_request_bench.cc"
    ],
    copts = [
        "-std=c++14",
    ],
    linkopts = [
        "-lprofiler",
        "-ltcmalloc",
        "-Wl,-no_pie",
    ],
    deps = [
        ":http",
        "@boost_1_62_0//:headers",
    ],
)
