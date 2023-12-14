load("//transpiler:fhe.bzl", "fhe_cc_library")

licenses(["notice"])

#define the transpiled function sources
load("helper.bzl", "gen_objects")

gen_objects(
    name = "intersect",
	common_headers =  "private_set.h"
)


gen_objects(
    name = "union",
	common_headers =  "private_set.h"	
)

gen_objects(
    name = "copy_outset_inc_flag",
	common_headers =  "private_set.h"	
)

gen_objects(
    name = "set_length",
	common_headers =  "private_set.h"	
)



#compiled non-transpiled function objects

cc_library(
    name = "parse_inputs",
    srcs = [
        "parse_inputs.cc",
    ],
    hdrs = [
        "private_set.h",
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
        "private_set.h",
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
        "private_set.h",
        "generate_data.h",
    ],
)


#compile and line executables

#openfhe

cc_binary(
    name = "private_set_openfhe_testbench",
    srcs = ["private_set_openfhe_testbench.cc"],
    copts = ["-DUSE_OPENFHE"], #here for triggering parse_inputs_openfhe
    deps = [
	    ":parse_inputs_openfhe",
	    ":generate_data",
		":intersect_openfhe",
        ":union_openfhe",
        ":copy_outset_inc_flag_openfhe",
        ":set_length_openfhe",
        "//transpiler/data:openfhe_data",
        "@com_google_xls//xls/common/logging",
        "@com_google_xls//xls/common/status:status_macros",
        "@openfhe//:binfhe",
    ],
)

cc_binary(
    name = "private_set_interpreted_openfhe_testbench",
    srcs = ["private_set_openfhe_testbench.cc"],
    copts = ["-DUSE_INTERPRETED_OPENFHE"],
    deps = [
	    ":parse_inputs_openfhe",
	    ":generate_data",
		":intersect_interpreted_openfhe",
		":union_interpreted_openfhe",
        ":copy_outset_inc_flag_interpreted_openfhe",
        ":set_length_interpreted_openfhe",
		"//transpiler/data:openfhe_data",
        "@com_google_xls//xls/common/logging",
        "@com_google_xls//xls/common/status:status_macros",
        "@openfhe//:binfhe",
    ],
)
cc_binary(
    name = "private_set_yosys_interpreted_openfhe_testbench",
    srcs = ["private_set_openfhe_testbench.cc"],
    copts = ["-DUSE_YOSYS_INTERPRETED_OPENFHE"],
    deps = [
	    ":parse_inputs_openfhe",
	    ":generate_data",
		":intersect_yosys_interpreted_openfhe",
        ":union_yosys_interpreted_openfhe",
        ":copy_outset_inc_flag_yosys_interpreted_openfhe",
        ":set_length_yosys_interpreted_openfhe",
        "//transpiler/data:openfhe_data",
        "@com_google_absl//absl/base",
        "@com_google_xls//xls/common/logging",
        "@com_google_xls//xls/common/status:status_macros",
        "@openfhe//:binfhe",
    ],
)

#cleartext

cc_binary(
    name = "private_set_cleartext_testbench",
    srcs = ["private_set_cleartext_testbench.cc"],
    deps = [
	    ":parse_inputs",
	    ":generate_data",
        ":intersect_cleartext",
        ":union_cleartext",
        ":copy_outset_inc_flag_cleartext",
        ":set_length_cleartext",
        "//transpiler/data:cleartext_data",
        "@com_google_xls//xls/common/logging",
        "@com_google_xls//xls/common/status:status_macros",
    ],
)

cc_binary(
    name = "private_set_yosys_interpreted_cleartext_testbench",
    srcs = ["private_set_cleartext_testbench.cc"],
    copts = ["-DUSE_YOSYS_INTERPRETED_CLEARTEXT"],
    deps = [
	    ":parse_inputs",
	    ":generate_data",
        ":intersect_yosys_interpreted_cleartext",
        ":union_yosys_interpreted_cleartext",
        ":copy_outset_inc_flag_yosys_interpreted_cleartext",
        ":set_length_yosys_interpreted_cleartext",
        "//transpiler/data:cleartext_data",
        "@com_google_xls//xls/common/logging",
        "@com_google_xls//xls/common/status:status_macros",
    ],
)
