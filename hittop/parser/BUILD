cc_library(
    name = "parser",
    hdrs = [
        "any_char.h",
        "at_least.h",
        "at_most.h",
        "between.h",
        "char_class.h",
        "char_filter.h",
        "concat.h",
        "either.h",
        "exactly.h",
        "failure.h",
        "forward_ref.h",
        "force.h",
        "implied_delim.h",
        "integer_parse_visitor.h",
        "inter.h",
        "literal.h",
        "opt.h",
        "parse_error.h",
        "parser.h",
        "repeat.h",
        "repeat_and_then.h",
        "success.h",
        "token.h",
        "trace_visitor.h",
        "traits.h",
        "trim.h",
        "unless.h",
    ],
    srcs = [
         "integer_parse_visitor.cc",
    ],
    copts = ["-std=c++14"],
    deps = [
        "//hittop/util:abi",
        "//hittop/util:util",
        "//hittop/util:functional",
        "//third_party/short_alloc",
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
        "exactly-test.cc",
        "failure-test.cc",
        "forward_ref-test.cc",
        "force-test.cc",
        "implied_delim-test.cc",
        "integer_parse_visitor-test.cc",
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
    name = "repeat_and_then-test",
    srcs = [
        "repeat_and_then-test.cc",
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
    name = "either-test",
    srcs = [
        "either-test.cc",
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
