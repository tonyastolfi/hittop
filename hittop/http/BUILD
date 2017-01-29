cc_library(
    name = "http",
    hdrs = [
        "rfc2616.h",
    ],
    copts = ["-std=c++14"],
    deps = [
        "//hittop/util:util",
        "//hittop/util:functional",
        "//hittop/parser",
        "@boost_1_62_0//:headers",
    ],
    visibility = ["//visibility:public"]
)

cc_test(
    name = "rfc-2616-test",
    srcs = [
        "rfc-2616-test.cc",
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
