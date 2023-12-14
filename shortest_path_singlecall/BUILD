load("//transpiler:fhe.bzl", "fhe_cc_library")

licenses(["notice"])

#define the transpiled function sources
load("helper.bzl", "gen_objects")

gen_objects(
    name = "compute_shortest_path",
	common_headers =  "shortest_path.h"
)

gen_objects(
    name = "and_test",
	common_headers =  "shortest_path.h"
)

#compile transpiled function objects


#compile and line executables

cc_binary(
    name = "shortest_path_tfhe_testbench",
    srcs = ["shortest_path_tfhe_testbench.cc"],
    deps = [
        ":shortest_path_tfhe",
        ":and_test_tfhe",
        "//transpiler/data:tfhe_data",
        "@com_google_xls//xls/common/logging",
        "@com_google_xls//xls/common/status:status_macros",
        "@tfhe//:libtfhe",
    ],
)

cc_binary(
    name = "shortest_path_interpreted_tfhe_testbench",
    srcs = ["shortest_path_tfhe_testbench.cc"],
    copts = ["-DUSE_INTERPRETED_TFHE"],
    deps = [
        ":compute_shortest_path_interpreted_tfhe",
        ":and_test_tfhe",
        "//transpiler/data:tfhe_data",
        "@com_google_xls//xls/common/logging",
        "@com_google_xls//xls/common/status:status_macros",
        "@tfhe//:libtfhe",
    ],
)

cc_binary(
    name = "shortest_path_openfhe_testbench",
    srcs = ["shortest_path_openfhe_testbench.cc"],
    deps = [
        ":and_test_openfhe",
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
        ":and_test_interpreted_openfhe",
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
        ":and_test_yosys_interpreted_openfhe",
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
        ":and_test_cleartext",
        ":compute_shortest_path_cleartext",
        "//transpiler/data:cleartext_data",
        "@com_google_xls//xls/common/logging",
        "@com_google_xls//xls/common/status:status_macros",
    ],
)

#compile and link google tests.

cc_test(
    name = "shortest_path_test",
    srcs = ["shortest_path_test.cc"],
    deps = [
        ":shortest_path",
        ":create_cost_vector",
        "@com_google_googletest//:gtest_main",
        "@com_google_xls//xls/common/status:matchers",
    ],
)
