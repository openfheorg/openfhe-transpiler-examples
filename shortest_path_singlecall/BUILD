load("//transpiler:fhe.bzl", "fhe_cc_library")

licenses(["notice"])

#define the transpiled function sources
load("helper.bzl", "gen_objects")

gen_objects(
    name = "compute_shortest_path",
	common_headers =  "shortest_path.h"
)

#compile transpiled function objects


#compile and line executables

cc_binary(
    name = "shortest_path_openfhe_testbench",
    srcs = ["shortest_path_openfhe_testbench.cc"],
    deps = [
		":compute_shortest_path_openfhe",
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
        ":compute_shortest_path_interpreted_openfhe",
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
        ":compute_shortest_path_yosys_interpreted_openfhe",
        "//transpiler/data:openfhe_data",
        "@com_google_absl//absl/base",
        "@com_google_xls//xls/common/logging",
        "@com_google_xls//xls/common/status:status_macros",
        "@openfhe//:binfhe",
    ],
)

cc_binary(
    name = "shortest_path_cleartext_testbench",
    srcs = ["shortest_path_cleartext_testbench.cc"],
    deps = [
        ":compute_shortest_path_cleartext",
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

        ":compute_shortest_path_yosys_interpreted_cleartext",

        "//transpiler/data:cleartext_data",
        "@com_google_xls//xls/common/logging",
        "@com_google_xls//xls/common/status:status_macros",
    ],
)
