import glob
import ninja_syntax
import sys
import os
from os import path

# ------------------------------------------------------------------------------

outfile = open("build.ninja", "w+")
ninja = ninja_syntax.Writer(outfile)

obj_dir = "obj"

"""
incdirs = [
    "src",
    "/usr/local/share/verilator/include",
    "/usr/local/share/verilator/include/vltstd",
    "tree-sitter/lib/src",
    "tree-sitter/lib/include",
]
ninja.variable("includes", ["-I" + d for d in incdirs])
"""

def swap_ext(name, new_ext):
  return path.splitext(name)[0] + new_ext

# ------------------------------------------------------------------------------

def compile_cpp(src_dir, dst_dir, src_files = None, variables = None):
    if (src_files is None):
      src_files = [path.basename(n) for n in glob.glob(path.join(src_dir, "*.cpp"))]
    for n in src_files:
        ninja.build(
            path.join(dst_dir, src_dir, swap_ext(n, ".o")),
            "compile_cpp",
            path.join(src_dir, n),
            variables=variables
        )

def compile_c(src_dir, dst_dir, src_files = None):
    for n in src_files:
        ninja.build(
            path.join(dst_dir, src_dir, swap_ext(n, ".o")),
            "compile_c",
            path.join(src_dir, n),
        )

def metronate_directory(src_dir, dst_dir):
    src_paths = glob.glob(path.join(src_dir, "*.h"))
    src_files = [path.basename(n) for n in src_paths]
    dst_paths = [path.join(dst_dir, swap_ext(n, ".sv")) for n in src_files]
    ninja.build(
        dst_paths,
        "metron",
        [],
        implicit=["bin/metron"] + src_paths,
        variables={
            "src_dir": src_dir,
            "dst_dir": dst_dir,
            "src_files": src_files
        }
    )

# ------------------------------------------------------------------------------

ninja.variable("opt", "-O0")

# -------------------------------------------------------------------------------

outfile.write("################################################################################\n");
outfile.write("# Rules\n\n");

ninja.rule(
    name="compile_cpp",
    command="g++ -g $opt -std=gnu++2a ${includes} -MMD -MF ${out}.d -c ${in} -o ${out}",
    deps="gcc",
    depfile="${out}.d",
)

ninja.rule(
    name="compile_c",
    command="gcc -g $opt ${includes} -MMD -MF ${out}.d -c ${in} -o ${out}",
    deps="gcc",
    depfile="${out}.d",
)

ninja.rule(
    name="link",
    command="g++ -g $opt ${in} ${link_libs} -o ${out}"
    # command = "g++ -g $opt -Wl,--whole-archive ${in} -Wl,--no-whole-archive ${link_libs} -o ${out}"
)

ninja.rule(
    name="iverilog",
    command="iverilog -g2012 -DIVERILOG ${in} -o ${out}"
)

ninja.rule(name="yosys",
           command="yosys -p 'read_verilog -sv ${in}; dump; synth_ice40 -json ${out};'")

ninja.rule(
    name="nextpnr-ice40",
    command="nextpnr-ice40 -q --hx8k --package ct256 --json ${in} --asc ${out} --pcf ${pcf}"
)

ninja.rule(
    name="iceprog",
    command="icepack ${in} ${out} && iceprog -S ${out}"
)

ninja.rule(
    name="run_test",
    command="${in} | grep \"All tests pass.\" && touch ${out}"
)

ninja.rule(
    name="run_cmd",
    command="${cmd}"
)

ninja.rule(
    name="sv2v",
    command="sv2v -w ${out} ${in}"
)

ninja.rule(
    name="run_in_console",
    command="${in}",
    pool="console"
)

ninja.rule(
    name="verilate",
    command="verilator $includes --cc $top -Mdir $out_dir"
)

ninja.rule(
    name="make",
    command="make -C $make_dir -f $makefile"
)

ninja.rule(
    name="metron",
    command="bin/metron -q -R$src_dir -O$dst_dir $src_files",
)

outfile.write("\n")
outfile.write("################################################################################\n");
outfile.write("# Verilator libraries\n\n");

ninja.build(
  path.join(obj_dir, "verilated.o"),
  "compile_cpp",
  "/usr/share/verilator/include/verilated.cpp",
  variables = {
    "includes" : ""
  }
)

outfile.write("\n")
outfile.write("################################################################################\n");
outfile.write("# TreeSitter libraries\n\n");

treesitter_srcs = [
    "tree-sitter/lib/src/lib.c",
    "tree-sitter-cpp/src/parser.c",
    "tree-sitter-cpp/src/scanner.cc",
]
treesitter_objs = []

for n in treesitter_srcs :
  o = path.join(obj_dir, swap_ext(n, ".o"))
  ninja.build(
    o,
    "compile_c",
    n,
    variables={
      "includes":"-Itree-sitter/lib/include"
    }
  )
  treesitter_objs.append(o)

outfile.write("\n")
outfile.write("################################################################################\n");
outfile.write("# Compile Metron itself\n\n");

metron_src_dir = "src"
metron_srcs = [
  "MetronApp.cpp",
  "MtCursor.cpp",
  "MtMethod.cpp",
  "MtModLibrary.cpp",
  "MtModule.cpp",
  "MtNode.cpp",
  "MtSourceFile.cpp",
  "Platform.cpp",
]
metron_objs = [path.join(obj_dir, metron_src_dir, swap_ext(n, ".o")) for n in metron_srcs]

compile_cpp(
  metron_src_dir,
  obj_dir,
  metron_srcs,
  variables={
    "includes":"-Itree-sitter/lib/include"
  }  
)
outfile.write("\n")
ninja.build(
  "bin/metron",
  "link",
  metron_objs + treesitter_objs
)

outfile.write("\n")
outfile.write("################################################################################\n");
outfile.write("# Regenerate SystemVerilog version of example_uart\n\n");

metronate_directory("example_uart/rtl", "example_uart/generated")
outfile.write("\n# ---------- ---------- ---------- ---------- \n\n")

""""
################################################################################
# Regenerate SystemVerilog version of example_rvsimple

metronate_directory("example_rvsimple/rtl", "example_rvsimple/generated")
outfile.write("\n# ---------- ---------- ---------- ---------- \n\n")

################################################################################
# Build example_uart/rtl/main.cpp

uart_rtl_dir = "example_uart/rtl"
uart_rtl_srcs = [path.basename(n) for n in glob.glob(path.join(uart_rtl_dir, "*.h"))]

ninja.build(
  "out/example_uart/rtl/main.o",
  "compile_cpp",
  "example_uart/rtl/main.cpp"
)

ninja.build(
  "bin/example_uart/rtl/main",
  "link",
  "out/example_uart/rtl/main.o",
)

################################################################################
# Verilate and build example_uart/generated/main.cpp

def verilate_dir(src_dir, dst_dir, src_top):
    src_paths = glob.glob(path.join(src_dir, "*.sv"))
    src_files = [path.basename(n) for n in src_paths]
    dst_paths = [path.join(dst_dir, swap_ext(n, ".sv")) for n in src_files]

    #command="verilator $includes --cc $top -Mdir $out_dir"

    ninja.build(
      [
        path.join(dst_dir, src_dir, swap_ext("V" + src_top, ".h")),
        path.join(dst_dir, src_dir, swap_ext("V" + src_top, ".mk")),
      ],
      "verilate",
      src_files,
      variables={
          "top": "uart_top.sv",
          "includes": "-I" + src_dir + " -Isrc",
          "out_dir": path.join(dst_dir, src_dir)
      }
    )

uart_gen_dir = "example_uart/generated"
uart_gen_srcs = [path.basename(n) for n in glob.glob(path.join(uart_gen_dir, "*.sv"))]

uart_gen_verilated = [
    "out/example_uart/generated/Vuart_top.h",
    "out/example_uart/generated/Vuart_top.mk",
]
uart_gen_main_obj = "out/example_uart/generated/main.o"
uart_gen_vl_obj = "out/example_uart/generated/Vuart_top__ALL.o"

ninja.build(
  uart_gen_main_obj,
  "compile_cpp",
  "example_uart/generated/main.cpp"
)

ninja.build(
  uart_gen_verilated,
  "verilate",
  uart_gen_srcs,
  variables={
      "top": "uart_top.sv",
      "includes": "-Iexample_uart/generated -Isrc",
      "out_dir": "out/example_uart/generated"
  }
)

ninja.build(
  uart_gen_vl_obj,
  "run_cmd",
  [],
  implicit=uart_gen_verilated,
  variables={
    "cmd": "make -C out/example_uart/generated -f Vuart_top.mk"
  }
)

ninja.build(
  "bin/example_uart/enerated/main",
  "link",
  [
    "out/example_uart/generated/main.o",
    uart_gen_vl_obj,
  ] + verilator_objs
)

################################################################################
# Build example_rvsimple/rtl/main.cpp

rvsimple_rtl_dir = "example_rvsimple/rtl"
rvsimple_rtl_srcs = [path.basename(n) for n in glob.glob(path.join(rvsimple_rtl_dir, "*.h"))]

ninja.build(
  "out/example_rvsimple/rtl/main.o",
  "compile_cpp",
  "example_rvsimple/rtl/main.cpp"
)

ninja.build(
  "bin/example_rvsimple/rtl/main",
  "link",
  "out/example_rvsimple/rtl/main.o",
)

################################################################################
# Verilate and build example_rvsimple/generated/main.cpp

rvsimple_gen_dir = "example_rvsimple/generated"
rvsimple_gen_srcs = [path.basename(n) for n in glob.glob(path.join(rvsimple_rtl_dir, "*.sv"))]

ninja.build(
  [
    "out/example_uart/generated/Vuart_top.h",
    "out/example_uart/generated/Vuart_top.mk",
  ],
  "verilate",
  [path.join(uart_gen_dir, n) for n in uart_gen_srcs],
  variables={
      "includes": "-Iexample_uart/generated -Isrc",
      "out_dir": "out/example_uart/generated"
  }
)

ninja.build(
  "out/example_uart/generated/Vuart_top__ALL.o",
  "run_cmd",
  [],
  variables={
    "cmd": "make -C out/example_uart/generated -f Vuart_top.mk"
  }
)

ninja.build(
  "bin/example_uart/generated/main",
  "link",
  [
    "out/example_uart/generated/main.o",
    "out/example_uart/generated/Vuart_top__ALL.o",
    "out/verilated.o",
  ]
)
"""


"""


dir = example_rvsimple/generated

build out/$dir/Vtoplevel.mk out/$dir/Vtoplevel.h : verilate $dir/toplevel.sv | $
  $dir/adder.sv $
  $dir/alu.sv $
  $dir/alu_control.sv $
  $dir/config.sv $
  $dir/constants.sv $
  $dir/control_transfer.sv $
  $dir/data_memory_interface.sv $
  $dir/example_data_memory.sv $
  $dir/example_data_memory_bus.sv $
  $dir/example_text_memory.sv $
  $dir/example_text_memory_bus.sv $
  $dir/immediate_generator.sv $
  $dir/instruction_decoder.sv $
  $dir/multiplexer.sv $
  $dir/multiplexer2.sv $
  $dir/multiplexer4.sv $
  $dir/multiplexer8.sv $
  $dir/regfile.sv $
  $dir/register.sv $
  $dir/riscv_core.sv $
  $dir/singlecycle_control.sv $
  $dir/singlecycle_ctlpath.sv $
  $dir/singlecycle_datapath.sv
    includes = -I$dir -Isrc
    out_dir = out/$dir

build out/$dir/main.o : compile $dir/main.cpp | out/example_rvsimple/generated/Vtoplevel.h
  includes = ${includes} -Iout/$dir

build out/$dir/Vtoplevel__ALL.o : make out/$dir/Vtoplevel.mk
  make_dir = out/$dir
  makefile = Vtoplevel.mk

build bin/$dir/main : link out/$dir/main.o out/$dir/Vtoplevel__ALL.o out/verilated.o

################################################################################
# Verilate and build example_rvsimple/reference/main.cpp

dir = example_rvsimple/reference

build out/$dir/Vtoplevel.mk out/$dir/Vtoplevel.h : verilate $dir/toplevel.sv | $
  $dir/adder.sv $
  $dir/alu.sv $
  $dir/alu_control.sv $
  $dir/config.sv $
  $dir/constants.sv $
  $dir/control_transfer.sv $
  $dir/data_memory_interface.sv $
  $dir/example_data_memory.sv $
  $dir/example_data_memory_bus.sv $
  $dir/example_text_memory.sv $
  $dir/example_text_memory_bus.sv $
  $dir/immediate_generator.sv $
  $dir/instruction_decoder.sv $
  $dir/multiplexer.sv $
  $dir/multiplexer2.sv $
  $dir/multiplexer4.sv $
  $dir/multiplexer8.sv $
  $dir/regfile.sv $
  $dir/register.sv $
  $dir/riscv_core.sv $
  $dir/singlecycle_control.sv $
  $dir/singlecycle_ctlpath.sv $
  $dir/singlecycle_datapath.sv
    includes = -I$dir -Isrc
    out_dir = out/$dir

build out/$dir/main.o : compile $dir/main.cpp | out/$dir/Vtoplevel.h
  includes = ${includes} -Iout/$dir

build out/$dir/Vtoplevel__ALL.o : make out/$dir/Vtoplevel.mk
  make_dir = out/$dir
  makefile = Vtoplevel.mk

build bin/$dir/main : link out/$dir/main.o out/$dir/Vtoplevel__ALL.o out/verilated.o

################################################################################

#build benchmark_reference : run_always bin/example_rvsimple/main_reference
#build benchmark_generated : run_always bin/example_rvsimple/main_generated
#build benchmark_rtl : run_always bin/example_rvsimple/main_rtl

#build benchmark : phony benchmark_reference benchmark_generated benchmark_rtl

################################################################################

#default bin/metron
#default bin/example_rvsimple/main_reference
#default bin/example_rvsimple/main_generated
#default bin/example_rvsimple/main_rtl
"""


outfile.write("\n# ---------- ---------- ---------- ---------- \n\n")

