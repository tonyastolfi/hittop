cc_library(
    name = "headers",
    hdrs = glob([
         "boost/**",
    ]),
    includes = ["."],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "system",
    hdrs = glob([
         "boost/**",
    ]),
    includes = ["."],
    linkopts = [
        "-L/Users/astolfi/projects/boost_1_65_1/bin.v2/libs/system/build/darwin-4.2.1/release/link-static/threading-multi",
        "-lboost_system",
    ],
    visibility = ["//visibility:public"],
)