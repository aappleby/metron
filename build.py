#!/usr/bin/env python3

#iwyu -std=c++20 -I. -Isymlinks metron/nodes/CNodeField.cpp

import glob
import ninja_syntax
import sys
import os
from os import path

obj_dir = "obj"
outfile = open("build.ninja", "w+")
ninja = ninja_syntax.Writer(outfile)

def sorted_glob(*args, **kwargs):
    return sorted(glob.glob(*args, **kwargs))

base_includes = [
    "-I.",
    #"-I/usr/local/share/verilator/include",
    "-Isymlinks",
    "-Isymlinks/metrolib",
    "-Isymlinks/matcheroni",
    #"-Isymlinks/CLI11/include",
    #"-Itests",
]

metrolib_src = [
    "symlinks/metrolib/metrolib/core/Platform.cpp",
    "symlinks/metrolib/metrolib/core/Utils.cpp",
    "symlinks/metrolib/metrolib/core/Err.cpp",
]

metron_src_lib   = sorted_glob("metron/*.cpp")
metron_src_nodes = sorted_glob("metron/nodes/*.cpp")
metron_src_main  = sorted_glob("metron/main/*.cpp")

# ------------------------------------------------------------------------------

def swap_ext(name, new_ext):
    return path.splitext(name)[0] + new_ext


def divider(text):
    outfile.write("\n")
    outfile.write("\n")
    outfile.write(
        "################################################################################\n")
    outfile.write(f"# {text}\n\n")

# ------------------------------------------------------------------------------

def main():
    global outfile
    print("Regenerating build.ninja...")
    ninja.variable("toolchain", "x86_64-linux-gnu")
    #ninja.include("symlinks/metrolib/ninja/rules.ninja")
    ninja.include("rules.ninja")
    ninja.newline()

    divider("Variables")

    if ("--release" in sys.argv) or ("-r" in sys.argv):
        ninja.variable("opts_cpp", "-rdynamic -O3 -MMD -std=gnu++2a")
        ninja.variable("opts_c",   "-rdynamic -O3 -MMD")
        ninja.variable("opts_ld",  "-O3")
    else:
        ninja.variable("opts_cpp", "-rdynamic -ggdb3 -O0 -MMD -Wall -Werror -Wsuggest-override -Wno-unused-function -Wno-sign-compare -Wno-unused-variable -Wno-unused-but-set-variable -std=gnu++2a")
        ninja.variable("opts_c",   "-rdynamic -ggdb3 -O0 -MMD -Wall -Werror -Wno-unused-function -Wno-sign-compare -Wno-unused-variable -Wno-unused-but-set-variable")
        ninja.variable("opts_ld",  "-O3")

    ninja.variable("base_includes", base_includes)

    ninja.variable("libraries", "-lgcc -lc")

    ninja.newline()
    ninja.newline()
    ninja.newline()

    build_metron_ems()
    build_verilator()
    build_metron_lib()
    build_metron_app()
    build_metron_test()
    build_rvtests()
    build_uart()
    build_rvsimple()
    # build_pinwheel()
    # build_ibex()
    build_pong()
    #build_j1()
    build_gb_spu()
    print("Done!")
    outfile.close()
    outfile = None
    #return os.system("ninja")

# ------------------------------------------------------------------------------

def metronize_dir(src_dir, src_top, dst_dir):
    """
    Convert all .h files in the source directory into .sv files in the destination directory.
    Returns an array of converted filenames.
    """
    divider(f"Metronize {src_dir} -> {dst_dir}")

    src_paths = sorted_glob(path.join(src_dir, "*.h"))
    dst_paths = []

    for src_path in src_paths:
        src_dir, src_name = path.split(src_path)
        dst_path = path.join(dst_dir, swap_ext(src_name, ".sv"))
        ninja.build(rule="metron",
                    inputs=[src_path],
                    implicit=["bin/metron"],
                    outputs=[dst_path])
        dst_paths.append(dst_path)

    return dst_paths


def verilate_dir(src_dir, src_files, src_top, dst_dir):
    """
    Run Verilator on all .sv files in the source directory, using "src_top" as
    the top module. Returns the full paths of "V<src_top>.h" and
    "V<src_top>__ALL.obj" in the destination directory.
    """
    divider(f"Verilate {src_top}@{src_dir} -> {dst_dir}")

    verilated_make = path.join(dst_dir, f"V{src_top}.mk")
    verilated_hdr = path.join(dst_dir, f"V{src_top}.h")
    verilated_obj = path.join(dst_dir, f"V{src_top}__ALL.o")

    # Verilate and generate makefile + header
    ninja.build(rule="verilator",
                inputs=src_files,
                outputs=[verilated_make, verilated_hdr],
                includes=[f"-I{src_dir}"],
                src_top=src_top,
                dst_dir=dst_dir)

    # Compile via makefile to generate object file
    ninja.build(rule="make",
                inputs=verilated_make,
                outputs=verilated_obj,
                dst_dir=dst_dir,
                makefile=f"V{src_top}.mk")

    return (verilated_hdr, verilated_obj)


def cpp_binary(bin_name, src_files, src_objs=None, deps=None, link_deps=None, **kwargs):
    """
    Compiles a C++ binary from the given source files.
    """
    if src_objs is None:
        src_objs = []
    if deps is None:
        deps = []
    if link_deps is None:
        link_deps = []

    divider(f"Compile {bin_name}")

    for n in src_files:
        obj_name = path.join(obj_dir, swap_ext(n, ".o"))
        ninja.build(outputs=obj_name,
                    rule="compile_cpp",
                    inputs=n,
                    implicit=deps,
                    variables=kwargs)
        src_objs.append(obj_name)
    ninja.build(outputs=bin_name,
                rule="c_binary",
                inputs=src_objs + link_deps,
                variables=kwargs)


def cpp_library(lib_name, src_files, src_objs=None, deps=None, **kwargs):
    """
    Compiles a C++ binary from the given source files.
    """
    if deps is None:
        deps = []

    if src_objs is None:
        src_objs = []

    divider(f"Create static library {lib_name}")

    for n in src_files:
        obj_name = path.join(obj_dir, swap_ext(n, ".o"))
        ninja.build(outputs=obj_name,
                    rule="compile_cpp",
                    inputs=n,
                    implicit=deps,
                    variables=kwargs)
        src_objs.append(obj_name)
    ninja.build(outputs=lib_name,
                rule="c_library",
                inputs=src_objs,
                variables=kwargs)


def iverilog_binary(bin_name, src_top, src_files, includes):
    ninja.build(rule="iverilog",
                inputs=src_top,
                implicit=src_files,
                outputs=bin_name,
                includes=includes,
                defines="-DIVERILOG")

# ------------------------------------------------------------------------------

def build_verilator():
    divider("Verilator libraries")

    ninja.build(rule="compile_cpp",
                inputs="/usr/local/share/verilator/include/verilated.cpp",
                outputs="obj/verilated.o")
    ninja.build(rule="compile_cpp",
                inputs="/usr/local/share/verilator/include/verilated_threads.cpp",
                outputs="obj/verilated_threads.o")

# ------------------------------------------------------------------------------
# Build Metron itself

def build_metron_lib():
    cpp_library(
        lib_name="bin/libmetron.a",
        src_files = metron_src_lib + metron_src_nodes + metrolib_src,
        includes=["${base_includes}"],
        src_objs=[],
    )

# ------------------------------------------------------------------------------

def build_metron_app():
    cpp_binary(
        bin_name="bin/metron",
        src_files = metron_src_main,
        includes=["${base_includes}", "-Isymlinks/CLI11/include"],
        link_deps=["bin/libmetron.a"],
    )

# ------------------------------------------------------------------------------
# Emscriptenization

def build_metron_ems():
    divider("Emscripten")

    # Not including a -O2 or -Os causes Emscripten's memory use to blow up :/

    ninja.variable(key="packager",
                   value="$$EMSDK/upstream/emscripten/tools/file_packager.py")

    all_test_headers = sorted_glob("tests/**/*.h", recursive=True)
    all_example_headers = sorted_glob("examples/**/*.h", recursive=True)

    ninja.build(outputs = ["docs/demo/examples.data"],
                rule="run_command",
                inputs=all_test_headers + all_example_headers,
                command="python3 ${packager} docs/demo/examples.data --no-node --js-output=docs/demo/examples.js --preload examples tests/metron_good tests/metron_bad --exclude *.cpp *.sv *.MD *.hex *.pcf *.v *.txt");


    ninja.build(outputs = ["docs/tutorial/tutorial_src.data"],
                rule="run_command",
                inputs=sorted_glob("examples/tutorial/*.h"),
                command="python3 ${packager} docs/tutorial/tutorial_src.data --no-node --js-output=docs/tutorial/tutorial_src.js --preload examples/tutorial examples/uart/metron");

    def cpp_binary2(bin_name, rule_compile, rule_link, src_files, src_objs, obj_dir, deps=None, link_deps=None, **kwargs):
        if src_objs is None:
            src_objs = []
        if deps is None:
            deps = []
        if link_deps is None:
            link_deps = []

        divider(f"Compile {bin_name} using {rule_compile} and {rule_link}")

        for n in src_files:
            obj_name = path.join(obj_dir, swap_ext(n, ".o"))
            ninja.build(outputs=obj_name,
                        rule=rule_compile,
                        inputs=n,
                        implicit=deps,
                        variables=kwargs)
            src_objs.append(obj_name)
        ninja.build(outputs=bin_name,
                    rule=rule_link,
                    inputs=src_objs + link_deps,
                    variables=kwargs)

    cpp_binary2(
        bin_name="docs/app/metron.js",
        rule_compile="ems_compile_cpp",
        rule_link="ems_binary",
        src_files= metron_src_main + metron_src_lib + metron_src_nodes + metrolib_src,
        src_objs=[],
        obj_dir = "wasm/obj",
        includes=["${base_includes}", "-Isymlinks/CLI11/include"],
    )

# ------------------------------------------------------------------------------
# Low-level tests

def build_metron_test():
    cpp_binary(
        bin_name="bin/tests/metron_test",
        src_files=[
            "tests/test_main.cpp",
            "tests/test_logic.cpp",
            "tests/test_utils.cpp",
        ],
        includes=["${base_includes}"],
        link_deps=["bin/libmetron.a"],
    )

# ------------------------------------------------------------------------------

def build_j1():
    cpp_binary(
        bin_name="bin/examples/j1",
        src_files=[
            "examples/j1/main.cpp",
        ],
        includes=["${base_includes}"],
        src_objs=[],
        link_deps=["bin/libmetron.a"],
    )
    j1_srcs = metronize_dir("examples/j1/metron", "j1.h", "examples/j1/metron_sv")

# ------------------------------------------------------------------------------

def build_gb_spu():
    gb_spu_srcs = metronize_dir("examples/gb_spu/metron", "MetroBoySPU2.h", "examples/gb_spu/metron_sv")
    gb_spu_vhdr, gb_spu_vobj = verilate_dir(
        src_dir="examples/gb_spu/metron_sv",
        src_files=gb_spu_srcs,
        src_top="MetroBoySPU2",
        dst_dir="gen/examples/gb_spu/metron_vl",
    )
    cpp_binary(
        bin_name="bin/examples/gb_spu",
        src_files=["examples/gb_spu/gb_spu_main.cpp"],
        includes=["${base_includes}", "-I/usr/local/share/verilator/include", "-Igen/examples/gb_spu"],
        src_objs=["obj/verilated.o", "obj/verilated_threads.o", gb_spu_vobj],
        deps=[gb_spu_vhdr],
        link_deps=["bin/libmetron.a"],
    )


# ------------------------------------------------------------------------------
# Simple Uart testbench

def build_uart():
    cpp_binary(
        bin_name="bin/examples/uart",
        src_files=[
            "examples/uart/main.cpp",
        ],
        includes=["${base_includes}"],
        link_deps=["bin/libmetron.a"],
    )

    uart_srcs = metronize_dir("examples/uart/metron", "uart_top.h",
                              "examples/uart/metron_sv")

    uart_vhdr, uart_vobj = verilate_dir(
        src_dir="examples/uart/metron_sv",
        src_files=uart_srcs,
        src_top="uart_top",
        dst_dir="gen/examples/uart/metron_vl",
    )

    cpp_binary(
        bin_name="bin/examples/uart_vl",
        src_files=["examples/uart/main_vl.cpp"],
        includes=["${base_includes}", "-I/usr/local/share/verilator/include", "-Igen/examples/uart"],
        src_objs=["obj/verilated.o", "obj/verilated_threads.o", uart_vobj],
        deps=[uart_vhdr],
        link_deps=["bin/libmetron.a"],
    )

    divider("Icarus Verilog uart testbench")

    uart_includes = [
        "-Iexamples/uart",
        "-Iexamples/uart/metron_sv"
    ]

    iverilog_binary(
        bin_name="bin/examples/uart_iv",
        src_top="examples/uart/uart_test_iv.sv",
        src_files=uart_srcs,
        includes=uart_includes)

    divider("Yosys/NextPNR uart testbench")

    ninja.build(rule="yosys",
                inputs="examples/uart/uart_test_ice40.sv",
                implicit=uart_srcs + ["examples/uart/SB_PLL40_CORE.v"],
                outputs="obj/examples/uart/uart_test_ice40.json",
                includes=uart_includes)

    ninja.build(rule="nextpnr-ice40",
                inputs="obj/examples/uart/uart_test_ice40.json",
                outputs="obj/examples/uart/uart_test_ice40.asc",
                chip="hx8k",
                package="ct256",
                pcf="examples/uart/ice40-hx8k-b-evn.pcf")

    ninja.build(rule="icepack",
                inputs="obj/examples/uart/uart_test_ice40.asc",
                outputs="obj/examples/uart/uart_test_ice40.bin")

# build out/uart_test_ice40.bin   : iceprog out/uart_test_ice40.asc

# ------------------------------------------------------------------------------
# Test binaries for the RISC-V cores.

def build_rvtests():
    src_files = sorted_glob("tests/rv_tests/*.S")
    dst_text = [swap_ext(f, ".text.vh") for f in src_files]
    dst_data = [swap_ext(f, ".data.vh") for f in src_files]

    ninja.build(rule="make",
                inputs="tests/rv_tests/makefile",
                implicit=src_files,
                outputs=dst_text + dst_data,
                dst_dir="tests/rv_tests",
                makefile="makefile")

# ------------------------------------------------------------------------------
# RVSimple

def build_rvsimple():
    mt_root = "examples/rvsimple/metron"
    sv_root = "examples/rvsimple/metron_sv"
    vl_root = "gen/examples/rvsimple/metron_vl"

    cpp_binary(
        bin_name="bin/examples/rvsimple",
        src_files=["examples/rvsimple/main.cpp"],
        includes=["${base_includes}", "-Isymlinks/CLI11/include"],
        link_deps=["bin/libmetron.a"],
    )

    sv_srcs = metronize_dir(mt_root, "toplevel.h", sv_root)

    vl_vhdr, vl_vobj = verilate_dir(
        src_dir=sv_root,
        src_files=sv_srcs,
        src_top="toplevel",
        dst_dir=vl_root,
    )

    cpp_binary(
        bin_name="bin/examples/rvsimple_vl",
        src_files=["examples/rvsimple/main_vl.cpp"],
        includes=["${base_includes}", "-I/usr/local/share/verilator/include", f"-I{vl_root}", "-Isymlinks/CLI11/include"],
        src_objs=["obj/verilated.o", "obj/verilated_threads.o", vl_vobj],
        deps=[vl_vhdr],
        link_deps=["bin/libmetron.a"],
    )

    ref_sv_root = "examples/rvsimple/reference_sv"
    ref_vl_root = "gen/examples/rvsimple/reference_vl"

    ref_vhdr, ref_vobj = verilate_dir(
        src_dir=ref_sv_root,
        src_files=sorted_glob(f"{ref_sv_root}/*.sv"),
        src_top="toplevel",
        dst_dir=ref_vl_root
    )

    cpp_binary(
        bin_name="bin/examples/rvsimple_ref",
        src_files=["examples/rvsimple/main_ref_vl.cpp"],
        includes=["${base_includes}", "-I/usr/local/share/verilator/include", f"-I{ref_vl_root}", "-Isymlinks/CLI11/include"],
        src_objs=["obj/verilated.o", "obj/verilated_threads.o", ref_vobj],
        deps=[ref_vhdr],
        link_deps=["bin/libmetron.a"],
    )


# ------------------------------------------------------------------------------
# Pinwheel

def build_pinwheel():
    mt_root = "examples/pinwheel/metron"
    sv_root = "examples/pinwheel/metron_sv"
    vl_root = "gen/examples/pinwheel/metron_vl"

    cpp_binary(
        bin_name="bin/examples/pinwheel",
        src_files=["examples/pinwheel/main.cpp"],
        includes=["${base_includes}", mt_root],
        link_deps=["bin/libmetron.a"],
    )

    """
    pinwheel_sv_srcs = metronize_dir(mt_root, "pinwheel.h", sv_root)

    pinwheel_vl_vhdr, pinwheel_vl_vobj = verilate_dir(
        src_dir=sv_root,
        src_files=pinwheel_sv_srcs,
        src_top="pinwheel",
        dst_dir=vl_root
    )

    cpp_binary(
        bin_name="bin/examples/pinwheel_vl",
        src_files=["examples/pinwheel/main_vl.cpp"],
        includes=base_includes + [vl_root],
        src_objs=["obj/verilated.o", "obj/verilated_threads.o", pinwheel_vl_vobj],
        deps=[pinwheel_vl_vhdr],
        link_deps=["bin/libmetron.a"],
    )
    """

# ------------------------------------------------------------------------------
# Ibex


def build_ibex():
    cpp_binary(
        bin_name="bin/examples/ibex",
        src_files=["examples/ibex/main.cpp"],
        includes=["${base_includes}"],
    )

# ------------------------------------------------------------------------------
# Pong


def build_pong():
    mt_root = "examples/pong/metron"
    sv_root = "examples/pong/metron_sv"
    vl_root = "gen/examples/pong/metron_vl"

    cpp_binary(
        bin_name="bin/examples/pong",
        src_files=["examples/pong/main.cpp"],
        includes=["${base_includes}"],
        libraries="${libraries} -lSDL2",
        link_deps=["bin/libmetron.a"],
    )

    metronized_src = metronize_dir(mt_root, "pong.h", sv_root)

# ------------------------------------------------------------------------------


""""
# build benchmark_reference : run_always bin/examples/rvsimple/main_reference
# build benchmark_generated : run_always bin/examples/rvsimple/main_generated
# build benchmark_rtl : run_always bin/examples/rvsimple/main_rtl

# build benchmark : phony benchmark_reference benchmark_generated benchmark_rtl

# default bin/metron
# default bin/examples/rvsimple/main_reference
# default bin/examples/rvsimple/main_generated
# default bin/examples/rvsimple/main_rtl
"""

if __name__ == "__main__":
    sys.exit(main())
