#!/usr/bin/env python3

import glob
import ninja_syntax
import sys
import os
from os import path

# ------------------------------------------------------------------------------

obj_dir = "obj"
outfile = open("build.ninja", "w+")
ninja = ninja_syntax.Writer(outfile)


def swap_ext(name, new_ext):
    return path.splitext(name)[0] + new_ext


def divider(text):
    outfile.write("\n")
    outfile.write("\n")
    outfile.write(
        "################################################################################\n")
    outfile.write(f"# {text}\n\n")


# ------------------------------------------------------------------------------

outfile.write(
    "################################################################################\n")
outfile.write("# Autoupdate this build.ninja from build.py.\n\n")

ninja.rule(name="autoupdate",
           command="python3 $in",
           generator=1)

ninja.build("build.ninja", "autoupdate", "build.py")


# ------------------------------------------------------------------------------

divider("Rules")

ninja.rule("compile_cpp", command="g++ -g ${opt} -std=gnu++2a ${includes} -MMD -MF ${out}.d -c ${in} -o ${out}", deps="gcc", depfile="${out}.d")
ninja.rule("compile_c",   command="gcc -g ${opt} ${includes} -MMD -MF ${out}.d -c ${in} -o ${out}", deps="gcc", depfile="${out}.d")
ninja.rule("metron",      command="bin/metron -q -R${src_dir} -O${dst_dir} ${file_list}")
ninja.rule("verilator",   command="verilator ${includes} --cc ${src_top} -Mdir ${dst_dir}")
ninja.rule("make",        command="make -C ${dst_dir} -f ${makefile}")
ninja.rule("link",        command="g++ -g $opt ${in} ${link_libs} -o ${out}")
ninja.rule("run_test",    command="${in} | grep \"All tests pass.\" && touch ${out}")
ninja.rule("console",     command="${in}", pool="console")

# command = "g++ -g $opt -Wl,--whole-archive ${in} -Wl,--no-whole-archive ${link_libs} -o ${out}"

def metronize_dir(src_dir, dst_dir):
    """
    Convert all .h files in the source directory into .sv files in the destination directory.
    Returns an array of converted filenames.
    """
    divider(f"Metronize {src_dir} -> {dst_dir}")
    src_paths = glob.glob(path.join(src_dir, "*.h"))
    src_files = [path.basename(n) for n in src_paths]
    dst_paths = [path.join(dst_dir, swap_ext(n, ".sv")) for n in src_files]

    ninja.build(rule="metron",
                inputs=src_paths,
                implicit="bin/metron",
                outputs=dst_paths,
                src_dir=src_dir,
                dst_dir=dst_dir,
                file_list=src_files)

    return dst_paths


def verilate_dir(src_dir, src_top, dst_dir):
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
                inputs=glob.glob(f"{src_dir}/*.sv"),
                outputs=[verilated_make, verilated_hdr],
                includes=["-Isrc", f"-I{src_dir}"],
                src_top=src_top,
                dst_dir=dst_dir)

    # Compile via makefile to generate object file
    ninja.build(rule="make",
                inputs=verilated_make,
                outputs=verilated_obj,
                dst_dir=dst_dir,
                makefile=f"V{src_top}.mk")

    return (verilated_hdr, verilated_obj)


def cpp_binary(bin_name, src_files, src_objs=None, deps=None, **kwargs):
    """
    Compiles a C++ binary from the given source files.
    """
    if src_objs is None: src_objs = []
    if deps is None: deps = []

    divider(f"Compile {bin_name}")
    for n in src_files:
        obj_name = path.join(obj_dir, swap_ext(n, ".o"))
        ninja.build(outputs=obj_name,
                    rule="compile_cpp",
                    inputs=n,
                    implicit=deps,
                    variables=kwargs)
        src_objs.append(obj_name)
    ninja.build(bin_name, "link", src_objs)

# ------------------------------------------------------------------------------
divider("Verilator libraries")

ninja.build(rule="compile_cpp",
            inputs="/usr/share/verilator/include/verilated.cpp",
            outputs="obj/verilated.o")

# ------------------------------------------------------------------------------
divider("TreeSitter libraries")

treesitter_srcs = [
    "tree-sitter/lib/src/lib.c",
    "tree-sitter-cpp/src/parser.c",
    "tree-sitter-cpp/src/scanner.cc",
]
treesitter_objs = []

for n in treesitter_srcs:
    o = path.join(obj_dir, swap_ext(n, ".o"))
    ninja.build(
        o,
        "compile_c",
        n,
        includes="-Itree-sitter/lib/include"
    )
    treesitter_objs.append(o)

# ------------------------------------------------------------------------------

cpp_binary(
    bin_name="bin/metron",
    src_files=[
        "src/MetronApp.cpp",
        "src/MtCursor.cpp",
        "src/MtMethod.cpp",
        "src/MtModLibrary.cpp",
        "src/MtModule.cpp",
        "src/MtNode.cpp",
        "src/MtSourceFile.cpp",
        "src/Platform.cpp",
    ],
    includes=["-Itree-sitter/lib/include"],
    src_objs=treesitter_objs,
)

# ------------------------------------------------------------------------------
# Simple Uart testbench

cpp_binary(
    bin_name="bin/examples/uart/metron/main",
    src_files=["examples/uart/metron/main.cpp"],
    includes=["-Isrc"],
    # FIXME Why the F does the build break if I don't pass an empty array here?
    src_objs=[],
)

uart_srcs = metronize_dir("examples/uart/metron", "examples/uart/metron_sv")

uart_vhdr, uart_vobj = verilate_dir(
    src_dir="examples/uart/metron_sv",
    src_top="uart_top",
    dst_dir=path.join(obj_dir, "examples/uart/metron_sv")
)

cpp_binary(
    bin_name="bin/examples/uart/metron_sv/main",
    src_files=["examples/uart/metron_sv/main.cpp"],
    includes=[
        "-Isrc",
        "-Itests",
        "-Iobj/examples/uart/metron_sv",
        "-I/usr/local/share/verilator/include"
    ],
    src_objs=["obj/verilated.o", uart_vobj],
    deps=[uart_vhdr]
)

divider("Icarus Verilog uart testbench")

uart_includes = [
    "-Isrc",
    "-Iexamples/uart",
    "-Iexamples/uart/metron_sv"
]

ninja.rule(name="iverilog",
           command="iverilog -g2012 ${defines} ${includes} ${in} -o ${out}")

ninja.build(rule="iverilog",
            inputs="examples/uart/uart_test_iv.sv",
            outputs="bin/examples/uart/metron_sv/uart_test_iv",
            includes=uart_includes,
            defines="-DIVERILOG")

divider("Yosys/NextPNR uart testbench")

ninja.rule(name="yosys",
           command="yosys -p 'read_verilog ${includes} -sv ${in}; dump; synth_ice40 -json ${out};'")

ninja.build(rule="yosys",
            inputs="examples/uart/uart_test_ice40.sv",
            outputs="obj/examples/uart/uart_test_ice40.json",
            includes=uart_includes)

ninja.rule(name="nextpnr-ice40",
           command="nextpnr-ice40 -q --${chip} --package ${package} --json ${in} --asc ${out} --pcf ${pcf}")

ninja.build(rule="nextpnr-ice40",
            inputs="obj/examples/uart/uart_test_ice40.json",
            outputs="obj/examples/uart/uart_test_ice40.asc",
            chip="hx8k",
            package="ct256",
            pcf="examples/uart/ice40-hx8k-b-evn.pcf")


ninja.rule(name="icepack", command="icepack ${in} ${out}")
ninja.build(rule="icepack",
            inputs="obj/examples/uart/uart_test_ice40.asc",
            outputs="obj/examples/uart/uart_test_ice40.bin")

# build out/uart_test_ice40.bin   : iceprog out/uart_test_ice40.asc

# ------------------------------------------------------------------------------
# RVSimple

cpp_binary(
    bin_name="bin/examples/rvsimple",
    src_files=["examples/rvsimple/main.cpp"],
    includes=["-Isrc"],
    opt="-O3",
)

rvsimple_metron_srcs = metronize_dir(
    "examples/rvsimple/metron", "examples/rvsimple/metron_sv")

rvsimple_metron_vhdr, rvsimple_metron_vobj = verilate_dir(
    src_dir="examples/rvsimple/metron_sv",
    src_top="toplevel",
    dst_dir=path.join(obj_dir, "examples/rvsimple/metron_sv")
)

cpp_binary(
    bin_name="bin/examples/rvsimple_sv",
    src_files=["examples/rvsimple/main_sv.cpp"],
    includes=[
        "-Isrc",
        "-Itests",
        "-Iobj/examples/rvsimple",
        "-I/usr/local/share/verilator/include"
    ],
    src_objs=["obj/verilated.o", rvsimple_metron_vobj],
    deps=[rvsimple_metron_vhdr]
)

rvsimple_reference_vhdr, rvsimple_reference_vobj = verilate_dir(
    src_dir="examples/rvsimple/reference",
    src_top="toplevel",
    dst_dir=path.join(obj_dir, "examples/rvsimple/reference")
)

cpp_binary(
    bin_name="bin/examples/rvsimple_ref",
    src_files=["examples/rvsimple/main_ref.cpp"],
    includes=[
        "-Isrc",
        "-Itests",
        "-Iobj/examples/rvsimple",
        "-I/usr/local/share/verilator/include"
    ],
    src_objs=["obj/verilated.o", rvsimple_reference_vobj],
    deps=[rvsimple_reference_vhdr]
)

# ------------------------------------------------------------------------------
# RVTiny

cpp_binary(
    bin_name="bin/examples/rvtiny",
    src_files=["examples/rvtiny/main.cpp"],
    includes=["-Isrc"],
    opt="-O3",
)

rvtiny_metron_srcs = metronize_dir(
    "examples/rvtiny/metron", "examples/rvtiny/metron_sv")

rvtiny_metron_vhdr, rvtiny_metron_vobj = verilate_dir(
    src_dir="examples/rvtiny/metron_sv",
    src_top="toplevel",
    dst_dir=path.join(obj_dir, "examples/rvtiny/metron_sv")
)

cpp_binary(
    bin_name="bin/examples/rvtiny_sv",
    src_files=["examples/rvtiny/main_sv.cpp"],
    includes=[
        "-Isrc",
        "-Itests",
        "-Iobj/examples/rvtiny",
        "-I/usr/local/share/verilator/include"
    ],
    src_objs=["obj/verilated.o", rvtiny_metron_vobj],
    deps=[rvtiny_metron_vhdr]
)

# ------------------------------------------------------------------------------

divider("Done!")


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
