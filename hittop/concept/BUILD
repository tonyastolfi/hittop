cc_library(
    name = "concept",
    hdrs = [
        "macros.h",
    ],
    srcs = [
    ],
    copts = ["-std=c++14"],
    deps = [
        "@boost//:headers",
        "@boost//:system",
        "//hittop/util",
        "//hittop/util:tuples",
    ],
    visibility = ["//visibility:public"]
)

cc_test(
    name = "concept-test",
    srcs = [
        "macros-test.cc",
    ],
    copts = [
        "-Iexternal/gtest/include",
        "-std=c++14",
    ],
    deps = [
        ":concept",
        "@gtest//:main",
        "@boost//:headers",
    ],
)
