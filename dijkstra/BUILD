load("//transpiler:fhe.bzl", "fhe_cc_library")

licenses(["notice"])

#define the transpiled function sources
load("helper.bzl", "gen_objects")

gen_objects(
    name = "min_distance",
	common_headers =  "dijkstra.h"
)

gen_objects(
    name = "set_src_distance",
	common_headers =  "dijkstra.h"	
)

gen_objects(
    name = "update_distance",
	common_headers =  "dijkstra.h"	
)

#compiled non-transpiled function objects
#inital cc only versions, will get transpiled.

cc_library(
    name = "update_distance_cc",
    srcs = [
        "update_distance.cc",
    ],
    hdrs = [
        "dijkstra.h",
        "update_distance.h",
    ],
)
cc_library(
    name = "set_src_distance_cc",
    srcs = [
        "set_src_distance.cc",
    ],
    hdrs = [
        "dijkstra.h",
        "set_src_distance.h",
    ],
)

cc_library(
    name = "min_distance_cc",
    srcs = [
        "min_distance.cc",
    ],
    hdrs = [
        "dijkstra.h",
        "min_distance.h",
    ],
)

cc_library(
    name = "parse_inputs_openfhe",
    srcs = [
        "parse_inputs.cc",
    ],
    copts = ["-DUSE_OPENFHE"],
    hdrs = [
        "dijkstra.h",
        "parse_inputs.h",
    ],
	deps = ["@openfhe//:binfhe",
	],	
)

#compile and line executables

#openfhe

cc_binary(
    name = "dijkstra_openfhe_testbench",
    srcs = ["dijkstra_openfhe_testbench.cc"],
    copts = ["-DUSE_OPENFHE"], #here for triggering parse_inputs_openfhe
    deps = [
	    ":parse_inputs_openfhe",
		":min_distance_openfhe",
        ":set_src_distance_openfhe",
        ":update_distance_openfhe",

        "//transpiler/data:openfhe_data",
        "@com_google_xls//xls/common/logging",
        "@com_google_xls//xls/common/status:status_macros",
        "@openfhe//:binfhe",
    ],
)

cc_binary(
    name = "dijkstra_interpreted_openfhe_testbench",
    srcs = ["dijkstra_openfhe_testbench.cc"],
    copts = ["-DUSE_INTERPRETED_OPENFHE"],
    deps = [
	    ":parse_inputs_openfhe",
		":min_distance_interpreted_openfhe",
        ":set_src_distance_interpreted_openfhe",
        ":update_distance_interpreted_openfhe",
		"//transpiler/data:openfhe_data",
        "@com_google_xls//xls/common/logging",
        "@com_google_xls//xls/common/status:status_macros",
        "@openfhe//:binfhe",
    ],
)
cc_binary(
    name = "dijkstra_yosys_interpreted_openfhe_testbench",
    srcs = ["dijkstra_openfhe_testbench.cc"],
    copts = ["-DUSE_YOSYS_INTERPRETED_OPENFHE"],
    deps = [
	    ":parse_inputs_openfhe",
		":min_distance_yosys_interpreted_openfhe",
        ":set_src_distance_yosys_interpreted_openfhe",
        ":update_distance_yosys_interpreted_openfhe",
        "//transpiler/data:openfhe_data",
        "@com_google_absl//absl/base",
        "@com_google_xls//xls/common/logging",
        "@com_google_xls//xls/common/status:status_macros",
        "@openfhe//:binfhe",
    ],
)

#cleartext

cc_binary(
    name = "dijkstra_cleartext_testbench",
    srcs = ["dijkstra_cleartext_testbench.cc"],
    deps = [
        ":min_distance_cc",
        ":update_distance_cc",
        ":min_distance_cleartext",
        ":set_src_distance_cleartext",
        ":update_distance_cleartext",
        "//transpiler/data:cleartext_data",
        "@com_google_xls//xls/common/logging",
        "@com_google_xls//xls/common/status:status_macros",
    ],
)

cc_binary(
    name = "dijkstra_yosys_interpreted_cleartext_testbench",
    srcs = ["dijkstra_cleartext_testbench.cc"],
    copts = ["-DUSE_YOSYS_INTERPRETED_CLEARTEXT"],
    deps = [
        ":min_distance_cc",
        ":update_distance_cc",
        ":min_distance_yosys_interpreted_cleartext",
        ":set_src_distance_yosys_interpreted_cleartext",
        ":update_distance_yosys_interpreted_cleartext",
        "//transpiler/data:cleartext_data",
        "@com_google_xls//xls/common/logging",
        "@com_google_xls//xls/common/status:status_macros",
    ],
)

#compile and link google tests.

#cc_test(
#    name = "dijkstra_test",
#    srcs = ["dijkstra_test.cc"],
#    deps = [
#        ":dijkstra",
#        ":create_cost_vector",
#        "@com_google_googletest//:gtest_main",
#        "@com_google_xls//xls/common/status:matchers",
#    ],
#)
