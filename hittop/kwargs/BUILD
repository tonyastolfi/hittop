cc_library(
    name = "kwargs",
    hdrs = [
        "kwargs.h",
    ],
    srcs = [
    ],
    copts = ["-std=c++14"],
    deps = [
        "@boost//:headers",
    ],
    visibility = ["//visibility:public"]
)

cc_test(
    name = "kwargs-test",
    srcs = [
        "kwargs-test.cc",
    ],
    copts = [
        "-Iexternal/gtest/include",
        "-std=c++14",
    ],
    deps = [
        "@gtest//:main",
        ":kwargs",
    ],
)
