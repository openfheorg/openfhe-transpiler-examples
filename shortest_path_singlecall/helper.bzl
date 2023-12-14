load("//transpiler:fhe.bzl",  "fhe_cc_library")

def gen_objects(name, common_headers, **kwargs):
  """
  Generate cc_library and various fhe_cc_library objects
  """


  native.cc_library(
    name = name,
    srcs = [name + ".cc"],
    hdrs = [common_headers,
            name + ".h",
	   ],
  )

#compile transpiled function objects

  fhe_cc_library(
    name = name+"_openfhe",
    src = name + ".cc",
    hdrs = [common_headers,
            name + ".h",
	   ],
    encryption = "openfhe",
    num_opt_passes = 2,
    optimizer = "xls",
  )


  fhe_cc_library(
    name = name + "_interpreted_openfhe",
    src = name + ".cc",
    hdrs = [common_headers,
            name + ".h",
	   ],
    encryption = "openfhe",
    interpreter = True,
    num_opt_passes = 2,
    optimizer = "xls",
  )

  fhe_cc_library(
    name = name + "_yosys_interpreted_openfhe",
    src = name + ".cc",
    hdrs = [common_headers,
            name + ".h",
			],
    encryption = "openfhe",
    interpreter = True,
    num_opt_passes = 2,
    optimizer = "yosys",
  )	

  fhe_cc_library(
    name = name + "_cleartext",
    src = name + ".cc",
    hdrs = [common_headers,
            name + ".h",
	   ],
    encryption = "cleartext",
    num_opt_passes = 2,
    optimizer = "xls",
  )

