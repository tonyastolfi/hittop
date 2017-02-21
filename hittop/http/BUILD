cc_library(
    name = "http",
    hdrs = [
        "grammar.h",
    ],
    copts = ["-std=c++14"],
    deps = [
        "//hittop/util:util",
        "//hittop/util:functional",
        "//hittop/uri",
        "//hittop/parser",
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
    ],
    data = [
        ":chrome_request.bin",
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
