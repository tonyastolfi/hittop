# -*- mode: python; -*-
cc_library(
    name = "http",
    hdrs = [
        "basic_header.h",
        "basic_request.h",
        "grammar.h",
        "http_method.h",
        "http_version.h",
        "mutable_headers.h",
        "mutable_message.h",
        "mutable_request.h",
        "parse_request.h",
        "request.h",
        "request_parse_visitor.h",
        "server.h",
        "server_connection.h",
        "server_connection_factory.h",
   ],
    copts = ["-std=c++1z"],
    deps = [
        ":server_config_cc_proto",
        "//hittop/concept:concept",
        "//hittop/concurrent:concurrent",
        "//hittop/io:io",
        "//hittop/kwargs:kwargs",
        "//hittop/util:util",
        "//hittop/util:functional",
        "//hittop/uri",
        "//hittop/parser",
        "//third_party/short_alloc",
        "@boost//:headers",
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
        "-std=c++1z",
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
        "-std=c++1z",
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
        "-std=c++1z",
    ],
    linkopts = [
        "-Wl,-no_pie",
    ],
    deps = [
        ":http",
        "@boost//:headers",
    ],
)

cc_binary(
    name = "static_server",
    srcs = [
        "static_server_main.cc",
    ],
    copts = [
        "-std=c++1z",
    ],
    deps = [
        ":http",
        "@boost//:headers",
    ]
)

proto_library(
    name = "server_config_proto",
    srcs = ["server_config.proto"],
    deps = [],
)

cc_proto_library(
    name = "server_config_cc_proto",
    deps = [":server_config_proto"],
)
