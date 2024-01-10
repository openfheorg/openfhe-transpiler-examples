load("//transpiler:fhe.bzl", "fhe_cc_library")

licenses(["notice"])

#define the transpiled function sources
load("helper.bzl", "gen_objects")

gen_objects(
    name = "AES_init_ctx",
	common_headers =  "aes.h", 
)

gen_objects(
    name = "AddRoundKey",
	common_headers =  "aes.h", 
)

gen_objects(
    name = "ShiftRows",
	common_headers =  "aes.h", 
)
gen_objects(
    name = "SubBytes",
	common_headers =  "aes.h", 
)
gen_objects(
    name = "MixColumns",
	common_headers =  "aes.h", 
)


gen_objects(
    name = "InvShiftRows",
	common_headers =  "aes.h", 
)
gen_objects(
    name = "InvSubBytes",
	common_headers =  "aes.h", 
)
gen_objects(
    name = "InvMixColumns",
	common_headers =  "aes.h", 
)

gen_objects(
    name = "XorWithIv",
	common_headers =  "aes.h", 
)

gen_objects(
    name = "XorWithBuff",
	common_headers =  "aes.h", 
)

gen_objects(
    name = "IncIvOF",
	common_headers =  "aes.h", 
)

#compile transpiled function objects


#compile and line executables
#########
#ECB

#########
#encrypted operation using openfhe and cleartext versions

cc_binary(
    name = "ecb_enc_openfhe_testbench",
    srcs = ["ecb_enc_openfhe_testbench.cc"],
    deps = [
        ":AES_init_ctx_openfhe",
        ":AddRoundKey_openfhe",
        ":ShiftRows_openfhe",
        ":SubBytes_openfhe",
        ":MixColumns_openfhe",
        "//transpiler/data:openfhe_data",
        "@com_google_xls//xls/common/logging",
        "@com_google_xls//xls/common/status:status_macros",
        "@openfhe//:binfhe",
    ],
)

cc_binary(
    name = "ecb_enc_interpreted_openfhe_testbench",
    srcs = ["ecb_enc_openfhe_testbench.cc"],
    copts = ["-DUSE_INTERPRETED_OPENFHE"],
    deps = [
        ":AES_init_ctx_interpreted_openfhe",
        ":AddRoundKey_interpreted_openfhe",
        ":ShiftRows_interpreted_openfhe",
        ":SubBytes_interpreted_openfhe",
        ":MixColumns_interpreted_openfhe",
		"//transpiler/data:openfhe_data",
        "@com_google_xls//xls/common/logging",
        "@com_google_xls//xls/common/status:status_macros",
        "@openfhe//:binfhe",
    ],
)
cc_binary(
    name = "ecb_enc_yosys_interpreted_openfhe_testbench",
    srcs = ["ecb_enc_openfhe_testbench.cc"],
    copts = ["-DUSE_YOSYS_INTERPRETED_OPENFHE"],
    deps = [
        ":AES_init_ctx_yosys_interpreted_openfhe",
        ":AddRoundKey_yosys_interpreted_openfhe",
        ":ShiftRows_yosys_interpreted_openfhe",
        ":SubBytes_yosys_interpreted_openfhe",
        ":MixColumns_yosys_interpreted_openfhe",
        "//transpiler/data:openfhe_data",
        "@com_google_absl//absl/base",
        "@com_google_xls//xls/common/logging",
        "@com_google_xls//xls/common/status:status_macros",
        "@openfhe//:binfhe",
    ],
)

cc_binary(
    name = "ecb_enc_cleartext_testbench",
    srcs = ["ecb_enc_cleartext_testbench.cc"],
    deps = [
        ":AES_init_ctx_cleartext",
        ":AddRoundKey_cleartext",
        ":ShiftRows_cleartext",
        ":SubBytes_cleartext",
        ":MixColumns_cleartext",
        "//transpiler/data:cleartext_data",
        "@com_google_xls//xls/common/logging",
        "@com_google_xls//xls/common/status:status_macros",
    ],
)



cc_binary(
    name = "ecb_enc_yosys_interpreted_cleartext_testbench",
    srcs = ["ecb_enc_cleartext_testbench.cc"],
 	copts = ["-DUSE_YOSYS_INTERPRETED_CLEARTEXT"],
    deps = [
        ":AES_init_ctx_yosys_interpreted_cleartext",
        ":AddRoundKey_yosys_interpreted_cleartext",
        ":ShiftRows_yosys_interpreted_cleartext",
        ":SubBytes_yosys_interpreted_cleartext",
        ":MixColumns_yosys_interpreted_cleartext",
        "//transpiler/data:cleartext_data",
        "@com_google_xls//xls/common/logging",
        "@com_google_xls//xls/common/status:status_macros",
    ],
)

##########
#decryption

#openfhe


cc_binary(
    name = "ecb_dec_openfhe_testbench",
    srcs = ["ecb_dec_openfhe_testbench.cc"],
    deps = [
        ":AES_init_ctx_openfhe",
        ":AddRoundKey_openfhe",
        ":InvShiftRows_openfhe",
        ":InvSubBytes_openfhe",
        ":InvMixColumns_openfhe",
        "//transpiler/data:openfhe_data",
        "@com_google_xls//xls/common/logging",
        "@com_google_xls//xls/common/status:status_macros",
        "@openfhe//:binfhe",
    ],
)

cc_binary(
    name = "ecb_dec_interpreted_openfhe_testbench",
    srcs = ["ecb_dec_openfhe_testbench.cc"],
    copts = ["-DUSE_INTERPRETED_OPENFHE"],
    deps = [
        ":AES_init_ctx_interpreted_openfhe",
        ":AddRoundKey_interpreted_openfhe",
        ":InvShiftRows_interpreted_openfhe",
        ":InvSubBytes_interpreted_openfhe",
        ":InvMixColumns_interpreted_openfhe",
		"//transpiler/data:openfhe_data",
        "@com_google_xls//xls/common/logging",
        "@com_google_xls//xls/common/status:status_macros",
        "@openfhe//:binfhe",
    ],
)
cc_binary(
    name = "ecb_dec_yosys_interpreted_openfhe_testbench",
    srcs = ["ecb_dec_openfhe_testbench.cc"],
    copts = ["-DUSE_YOSYS_INTERPRETED_OPENFHE"],
    deps = [
        ":AES_init_ctx_yosys_interpreted_openfhe",
        ":AddRoundKey_yosys_interpreted_openfhe",
        ":InvShiftRows_yosys_interpreted_openfhe",
        ":InvSubBytes_yosys_interpreted_openfhe",
        ":InvMixColumns_yosys_interpreted_openfhe",
        "//transpiler/data:openfhe_data",
        "@com_google_absl//absl/base",
        "@com_google_xls//xls/common/logging",
        "@com_google_xls//xls/common/status:status_macros",
        "@openfhe//:binfhe",
    ],
)

cc_binary(
    name = "ecb_dec_cleartext_testbench",
    srcs = ["ecb_dec_cleartext_testbench.cc"],
    deps = [
        ":AES_init_ctx_cleartext",
        ":AddRoundKey_cleartext",
        ":InvShiftRows_cleartext",
        ":InvSubBytes_cleartext",
        ":InvMixColumns_cleartext",
        "//transpiler/data:cleartext_data",
        "@com_google_xls//xls/common/logging",
        "@com_google_xls//xls/common/status:status_macros",
    ],
)


cc_binary(
    name = "ecb_dec_yosys_interpreted_cleartext_testbench",
    srcs = ["ecb_dec_cleartext_testbench.cc"],
 	copts = ["-DUSE_YOSYS_INTERPRETED_CLEARTEXT"],
    deps = [
        ":AES_init_ctx_yosys_interpreted_cleartext",
        ":AddRoundKey_yosys_interpreted_cleartext",
        ":InvShiftRows_yosys_interpreted_cleartext",
        ":InvSubBytes_yosys_interpreted_cleartext",
        ":InvMixColumns_yosys_interpreted_cleartext",
        "//transpiler/data:cleartext_data",
        "@com_google_xls//xls/common/logging",
        "@com_google_xls//xls/common/status:status_macros",
    ],
)

###########################################################################
#CBC

#encryption 

#openfhe

cc_binary(
    name = "cbc_enc_openfhe_testbench",
    srcs = ["cbc_enc_openfhe_testbench.cc"],
    deps = [
        ":AES_init_ctx_openfhe",
        ":AddRoundKey_openfhe",
        ":ShiftRows_openfhe",
        ":SubBytes_openfhe",
        ":MixColumns_openfhe",
		":XorWithIv_openfhe",
"//transpiler/data:openfhe_data",
        "@com_google_xls//xls/common/logging",
        "@com_google_xls//xls/common/status:status_macros",
        "@openfhe//:binfhe",
    ],
)

cc_binary(
    name = "cbc_enc_interpreted_openfhe_testbench",
    srcs = ["cbc_enc_openfhe_testbench.cc"],
    copts = ["-DUSE_INTERPRETED_OPENFHE"],
    deps = [
        ":AES_init_ctx_interpreted_openfhe",
        ":AddRoundKey_interpreted_openfhe",
        ":ShiftRows_interpreted_openfhe",
        ":SubBytes_interpreted_openfhe",
        ":MixColumns_interpreted_openfhe",
		":XorWithIv_interpreted_openfhe",
		"//transpiler/data:openfhe_data",
        "@com_google_xls//xls/common/logging",
        "@com_google_xls//xls/common/status:status_macros",
        "@openfhe//:binfhe",
    ],
)
cc_binary(
    name = "cbc_enc_yosys_interpreted_openfhe_testbench",
    srcs = ["cbc_enc_openfhe_testbench.cc"],
    copts = ["-DUSE_YOSYS_INTERPRETED_OPENFHE"],
    deps = [
        ":AES_init_ctx_yosys_interpreted_openfhe",
        ":AddRoundKey_yosys_interpreted_openfhe",
        ":ShiftRows_yosys_interpreted_openfhe",
        ":SubBytes_yosys_interpreted_openfhe",
        ":MixColumns_yosys_interpreted_openfhe",
		":XorWithIv_yosys_interpreted_openfhe",
        "//transpiler/data:openfhe_data",
        "@com_google_absl//absl/base",
        "@com_google_xls//xls/common/logging",
        "@com_google_xls//xls/common/status:status_macros",
        "@openfhe//:binfhe",
    ],
)

cc_binary(
    name = "cbc_enc_cleartext_testbench",
    srcs = ["cbc_enc_cleartext_testbench.cc"],
    deps = [
        ":AES_init_ctx_cleartext",
        ":AddRoundKey_cleartext",
        ":ShiftRows_cleartext",
        ":SubBytes_cleartext",
        ":MixColumns_cleartext",
		":XorWithIv_cleartext",
        "//transpiler/data:cleartext_data",
        "@com_google_xls//xls/common/logging",
        "@com_google_xls//xls/common/status:status_macros",
    ],
)


cc_binary(
    name = "cbc_enc_yosys_interpreted_cleartext_testbench",
    srcs = ["cbc_enc_cleartext_testbench.cc"],
 	copts = ["-DUSE_YOSYS_INTERPRETED_CLEARTEXT"],
    deps = [
        ":AES_init_ctx_yosys_interpreted_cleartext",
        ":AddRoundKey_yosys_interpreted_cleartext",
        ":ShiftRows_yosys_interpreted_cleartext",
        ":SubBytes_yosys_interpreted_cleartext",
  		":MixColumns_yosys_interpreted_cleartext",
		":XorWithIv_yosys_interpreted_cleartext",
        "//transpiler/data:cleartext_data",
        "@com_google_xls//xls/common/logging",
        "@com_google_xls//xls/common/status:status_macros",
    ],
)

##########
#decryption

#openfhe


cc_binary(
    name = "cbc_dec_openfhe_testbench",
    srcs = ["cbc_dec_openfhe_testbench.cc"],
    deps = [
        ":AES_init_ctx_openfhe",
        ":AddRoundKey_openfhe",
        ":InvShiftRows_openfhe",
        ":InvSubBytes_openfhe",
        ":InvMixColumns_openfhe",
		":XorWithIv_openfhe",
        "//transpiler/data:openfhe_data",
        "@com_google_xls//xls/common/logging",
        "@com_google_xls//xls/common/status:status_macros",
        "@openfhe//:binfhe",
    ],
)

cc_binary(
    name = "cbc_dec_interpreted_openfhe_testbench",
    srcs = ["cbc_dec_openfhe_testbench.cc"],
    copts = ["-DUSE_INTERPRETED_OPENFHE"],
    deps = [
        ":AES_init_ctx_interpreted_openfhe",
        ":AddRoundKey_interpreted_openfhe",
        ":InvShiftRows_interpreted_openfhe",
        ":InvSubBytes_interpreted_openfhe",
        ":InvMixColumns_interpreted_openfhe",
		":XorWithIv_interpreted_openfhe",
		"//transpiler/data:openfhe_data",
        "@com_google_xls//xls/common/logging",
        "@com_google_xls//xls/common/status:status_macros",
        "@openfhe//:binfhe",
    ],
)
cc_binary(
    name = "cbc_dec_yosys_interpreted_openfhe_testbench",
    srcs = ["cbc_dec_openfhe_testbench.cc"],
    copts = ["-DUSE_YOSYS_INTERPRETED_OPENFHE"],
    deps = [
        ":AES_init_ctx_yosys_interpreted_openfhe",
        ":AddRoundKey_yosys_interpreted_openfhe",
        ":InvShiftRows_yosys_interpreted_openfhe",
        ":InvSubBytes_yosys_interpreted_openfhe",
        ":InvMixColumns_yosys_interpreted_openfhe",
		":XorWithIv_yosys_interpreted_openfhe",
        "//transpiler/data:openfhe_data",
        "@com_google_absl//absl/base",
        "@com_google_xls//xls/common/logging",
        "@com_google_xls//xls/common/status:status_macros",
        "@openfhe//:binfhe",
    ],
)

cc_binary(
    name = "cbc_dec_cleartext_testbench",
    srcs = ["cbc_dec_cleartext_testbench.cc"],
    deps = [
        ":AES_init_ctx_cleartext",
        ":AddRoundKey_cleartext",
        ":InvShiftRows_cleartext",
        ":InvSubBytes_cleartext",
        ":InvMixColumns_cleartext",
		":XorWithIv_cleartext",
        "//transpiler/data:cleartext_data",
        "@com_google_xls//xls/common/logging",
        "@com_google_xls//xls/common/status:status_macros",
    ],
)


cc_binary(
    name = "cbc_dec_yosys_interpreted_cleartext_testbench",
    srcs = ["cbc_dec_cleartext_testbench.cc"],
 	copts = ["-DUSE_YOSYS_INTERPRETED_CLEARTEXT"],
    deps = [
        ":AES_init_ctx_yosys_interpreted_cleartext",
        ":AddRoundKey_yosys_interpreted_cleartext",
        ":InvShiftRows_yosys_interpreted_cleartext",
        ":InvSubBytes_yosys_interpreted_cleartext",
        ":InvMixColumns_yosys_interpreted_cleartext",
		":XorWithIv_yosys_interpreted_cleartext",
        "//transpiler/data:cleartext_data",
        "@com_google_xls//xls/common/logging",
        "@com_google_xls//xls/common/status:status_macros",
    ],
)
##################################################################
# CTR


#########
#encryption and decryption use the same code.

#openfhe

cc_binary(
    name = "ctr_xcr_openfhe_testbench",
    srcs = ["ctr_xcr_openfhe_testbench.cc"],
    deps = [
        ":AES_init_ctx_openfhe",
        ":AddRoundKey_openfhe",
        ":ShiftRows_openfhe",
        ":SubBytes_openfhe",
        ":MixColumns_openfhe",
		":IncIvOF_openfhe",
		":XorWithBuff_openfhe",
"//transpiler/data:openfhe_data",
        "@com_google_xls//xls/common/logging",
        "@com_google_xls//xls/common/status:status_macros",
        "@openfhe//:binfhe",
    ],
)

cc_binary(
    name = "ctr_xcr_interpreted_openfhe_testbench",
    srcs = ["ctr_xcr_openfhe_testbench.cc"],
    copts = ["-DUSE_INTERPRETED_OPENFHE"],
    deps = [
        ":AES_init_ctx_interpreted_openfhe",
        ":AddRoundKey_interpreted_openfhe",
        ":ShiftRows_interpreted_openfhe",
        ":SubBytes_interpreted_openfhe",
        ":MixColumns_interpreted_openfhe",
		":IncIvOF_interpreted_openfhe",
		":XorWithBuff_interpreted_openfhe",
		"//transpiler/data:openfhe_data",
        "@com_google_xls//xls/common/logging",
        "@com_google_xls//xls/common/status:status_macros",
        "@openfhe//:binfhe",
    ],
)
cc_binary(
    name = "ctr_xcr_yosys_interpreted_openfhe_testbench",
    srcs = ["ctr_xcr_openfhe_testbench.cc"],
    copts = ["-DUSE_YOSYS_INTERPRETED_OPENFHE"],
    deps = [
        ":AES_init_ctx_yosys_interpreted_openfhe",
        ":AddRoundKey_yosys_interpreted_openfhe",
        ":ShiftRows_yosys_interpreted_openfhe",
        ":SubBytes_yosys_interpreted_openfhe",
        ":MixColumns_yosys_interpreted_openfhe",
		":IncIvOF_yosys_interpreted_openfhe",
		":XorWithBuff_yosys_interpreted_openfhe",
        "//transpiler/data:openfhe_data",
        "@com_google_absl//absl/base",
        "@com_google_xls//xls/common/logging",
        "@com_google_xls//xls/common/status:status_macros",
        "@openfhe//:binfhe",
    ],
)

#cleartext

cc_binary(
    name = "ctr_xcr_cleartext_testbench",
    srcs = ["ctr_xcr_cleartext_testbench.cc"],
    deps = [
        ":AES_init_ctx_cleartext",
        ":AddRoundKey_cleartext",
        ":ShiftRows_cleartext",
        ":SubBytes_cleartext",
        ":MixColumns_cleartext",
		":IncIvOF_cleartext",
		":XorWithBuff_cleartext",
        "//transpiler/data:cleartext_data",
        "@com_google_xls//xls/common/logging",
        "@com_google_xls//xls/common/status:status_macros",
    ],
)


cc_binary(
    name = "ctr_xcr_yosys_interpreted_cleartext_testbench",
    srcs = ["ctr_xcr_cleartext_testbench.cc"],
 	copts = ["-DUSE_YOSYS_INTERPRETED_CLEARTEXT"],
    deps = [
        ":AES_init_ctx_yosys_interpreted_cleartext",
        ":AddRoundKey_yosys_interpreted_cleartext",
        ":ShiftRows_yosys_interpreted_cleartext",
        ":SubBytes_yosys_interpreted_cleartext",
  		":MixColumns_yosys_interpreted_cleartext",
		":IncIvOF_yosys_interpreted_cleartext",
		":XorWithBuff_yosys_interpreted_cleartext",
        "//transpiler/data:cleartext_data",
        "@com_google_xls//xls/common/logging",
        "@com_google_xls//xls/common/status:status_macros",
    ],
)


#compile and link google tests.

#Note we do not have any google tests for this code

#cc_test(
#    name = "aes_test",
#    srcs = ["aes_test.cc"],
#    deps = [
#        ":aes",
#        "@com_google_googletest//:gtest_main",
#        "@com_google_xls//xls/common/status:matchers",
#    ],
#)
