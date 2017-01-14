cc_library(
    name = "parser",
    hdrs = [
        "any_char.h",
        "at_least.h",
        "at_most.h",
        "between.h",
        "char_filter.h",
        "concat.h",
        "either.h",
        "exactly.h",
        "forward_ref.h",
        "force.h",
        "inter.h",
        "json_org.h",
        "literal.h",
        "opt.h",
        "parse_error.h",
        "parser.h",
        "repeat.h",
        "success.h",
        "token.h",
        "trim.h",
        "unless.h",
    ],
    copts = ["-std=c++14"],
    deps = [
        "@boost_1_62_0//:headers",
    ],
    visibility = ["//visibility:public"]
)

cc_test(
    name = "parser-test",
    srcs = [
        "any_char-test.cc",
        "at_least-test.cc",
        "at_most-test.cc",
        "between-test.cc",
        "char_filter-test.cc",
        "concat-test.cc",
        "either-test.cc",
        "exactly-test.cc",
        "forward_ref-test.cc",
        "force-test.cc",
        "inter-test.cc",
        "literal-test.cc",
        "opt-test.cc",
        "parse_error-test.cc",
        "repeat-test.cc",
        "success-test.cc",
        "token-test.cc",
        "trim-test.cc",
        "unless-test.cc",
    ],
    copts = [
        "-Iexternal/gtest/include",
        "-std=c++14",
    ],
    deps = [
        "@gtest//:main",
        ":parser",
        "//hittop/util",
    ],
)

cc_test(
    name = "json_org-test",
    srcs = [
        "json_org-test.cc",
    ],
    data = [
        ":json_org-test-data.json"
    ],
    copts = [
        "-Iexternal/gtest/include",
        "-std=c++14",
    ],
    deps = [
        "@gtest//:main",
        ":parser",
        "//hittop/util:util",
        "//hittop/util:test_util",
   ],
)
