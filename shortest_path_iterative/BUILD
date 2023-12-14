load("//transpiler:fhe.bzl", "fhe_cc_library")

licenses(["notice"])

#define the transpiled function sources
load("helper.bzl", "gen_objects")

gen_objects(
    name = "constrain_path",
	common_headers =  "shortest_path.h"
)

gen_objects(
    name = "select_path",
	common_headers =  "shortest_path.h"	
)

gen_objects(
    name = "update_path",
	common_headers =  "shortest_path.h"	
)

#compiled non-transpiled function objects

cc_library(
    name = "parse_inputs",
    srcs = [
        "parse_inputs.cc",
    ],
    hdrs = [
        "shortest_path.h",
        "parse_inputs.h",
    ],
)

cc_library(
    name = "parse_inputs_openfhe",
    srcs = [
        "parse_inputs.cc",
    ],
    copts = ["-DUSE_OPENFHE"],
    hdrs = [
        "shortest_path.h",
        "parse_inputs.h",
    ],
	deps = ["@openfhe//:binfhe",
	],	
)

cc_library(
    name = "generate_data",
    srcs = [
        "generate_data.cc",
    ],
    hdrs = [
        "shortest_path.h",
        "generate_data.h",
    ],
)


#compile and line executables

#openfhe

cc_binary(
    name = "shortest_path_openfhe_testbench",
    srcs = ["shortest_path_openfhe_testbench.cc"],
    copts = ["-DUSE_OPENFHE"], #here for triggering parse_inputs_openfhe
    deps = [
	    ":parse_inputs_openfhe",
	    ":generate_data",
		":constrain_path_openfhe",
        ":select_path_openfhe",
		":update_path_openfhe",
        "//transpiler/data:openfhe_data",
        "@com_google_xls//xls/common/logging",
        "@com_google_xls//xls/common/status:status_macros",
        "@openfhe//:binfhe",
    ],
)

cc_binary(
    name = "shortest_path_interpreted_openfhe_testbench",
    srcs = ["shortest_path_openfhe_testbench.cc"],
    copts = ["-DUSE_INTERPRETED_OPENFHE"],
    deps = [
	    ":parse_inputs_openfhe",
	    ":generate_data",
		":constrain_path_interpreted_openfhe",
        ":select_path_interpreted_openfhe",
		":update_path_interpreted_openfhe",
		"//transpiler/data:openfhe_data",
        "@com_google_xls//xls/common/logging",
        "@com_google_xls//xls/common/status:status_macros",
        "@openfhe//:binfhe",
    ],
)
cc_binary(
    name = "shortest_path_yosys_interpreted_openfhe_testbench",
    srcs = ["shortest_path_openfhe_testbench.cc"],
    copts = ["-DUSE_YOSYS_INTERPRETED_OPENFHE"],
    deps = [
	    ":parse_inputs_openfhe",
	    ":generate_data",
		":constrain_path_yosys_interpreted_openfhe",
        ":select_path_yosys_interpreted_openfhe",
		":update_path_yosys_interpreted_openfhe",
        "//transpiler/data:openfhe_data",
        "@com_google_absl//absl/base",
        "@com_google_xls//xls/common/logging",
        "@com_google_xls//xls/common/status:status_macros",
        "@openfhe//:binfhe",
    ],
)

#cleartext

cc_binary(
    name = "shortest_path_cleartext_testbench",
    srcs = ["shortest_path_cleartext_testbench.cc"],
    deps = [
	    ":parse_inputs",
	    ":generate_data",
        ":constrain_path_cleartext",
        ":select_path_cleartext",
		":update_path_cleartext",
        "//transpiler/data:cleartext_data",
        "@com_google_xls//xls/common/logging",
        "@com_google_xls//xls/common/status:status_macros",
    ],
)

cc_binary(
    name = "shortest_path_yosys_interpreted_cleartext_testbench",
    srcs = ["shortest_path_cleartext_testbench.cc"],
    copts = ["-DUSE_YOSYS_INTERPRETED_CLEARTEXT"],
    deps = [
	    ":parse_inputs",
	    ":generate_data",
        ":constrain_path_yosys_interpreted_cleartext",
        ":select_path_yosys_interpreted_cleartext",
		":update_path_yosys_interpreted_cleartext",
        "//transpiler/data:cleartext_data",
        "@com_google_xls//xls/common/logging",
        "@com_google_xls//xls/common/status:status_macros",
    ],
)

#compile and link google tests.

#cc_test(
#    name = "shortest_path_test",
#    srcs = ["shortest_path_test.cc"],
#    deps = [
#        ":shortest_path",
#        ":create_cost_vector",
#        "@com_google_googletest//:gtest_main",
#        "@com_google_xls//xls/common/status:matchers",
#    ],
#)
