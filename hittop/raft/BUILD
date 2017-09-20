cc_library(
    name = "raft",
    hdrs = [
    ],
    deps = [
        "//hittop/util:util",
        "//hittop/util:functional",
        "//third_party/short_alloc",
        "@boost_1_62_0//:headers",
    ],
    visibility = ["//visibility:public"]
)

cc_test(
    name = "raft-test",
    srcs = [
    ],
    copts = [
        "-Iexternal/gtest/include",
        "-std=c++14",
    ],
    deps = [
        "@gtest//:main",
        ":raft",
        "//hittop/util",
    ],
)
