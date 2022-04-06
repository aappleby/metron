#!/usr/bin/env python3

import glob
import ninja_syntax
import sys
import os
from os import path

# ------------------------------------------------------------------------------


def main():
    print("Regenerating build.ninja...")
    build_verilator()
    build_treesitter()
    build_metron()
    build_rvtests()
    build_uart()
    build_rvsimple()
    build_rvtiny()
    build_rvtiny_sync()
    #build_ibex()
    build_pong()
    print("Done!")

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

ninja.rule("compile_cpp",
           command="g++ -g ${opt} -std=gnu++2a ${includes} -MMD -MF ${out}.d -c ${in} -o ${out}", deps="gcc", depfile="${out}.d")
ninja.rule("compile_c",
           command="gcc -g ${opt} ${includes} -MMD -MF ${out}.d -c ${in} -o ${out}", deps="gcc", depfile="${out}.d")
ninja.rule("metron",
           command="bin/metron -q -r ${src_dir} -o ${dst_dir} -c ${file_list}")
ninja.rule("verilator",
           command="verilator ${includes} --cc ${src_top} -Mdir ${dst_dir}")
ninja.rule("make",          command="make -C ${dst_dir} -f ${makefile}")
ninja.rule("link",          command="g++ -g $opt ${in} ${link_libs} -o ${out}")
ninja.rule("run_test",
           command="${in} | grep \"All tests pass.\" && touch ${out}")
ninja.rule("console",       command="${in}", pool="console")
ninja.rule("iverilog",
           command="iverilog -g2012 ${defines} ${includes} ${in} -o ${out}")
ninja.rule("yosys",
           command="yosys -p 'read_verilog ${includes} -sv ${in}; dump; synth_ice40 -json ${out};'")
ninja.rule("nextpnr-ice40",
           command="nextpnr-ice40 -q --${chip} --package ${package} --json ${in} --asc ${out} --pcf ${pcf}")
ninja.rule("icepack",       command="icepack ${in} ${out}")


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
    if src_objs is None:
        src_objs = []
    if deps is None:
        deps = []

    divider(f"Compile {bin_name}")

    # Tack -I onto the includes
    if kwargs["includes"] is not None:
        kwargs["includes"] = ["-I" + path for path in kwargs["includes"]]

    for n in src_files:
        obj_name = path.join(obj_dir, swap_ext(n, ".o"))
        ninja.build(outputs=obj_name,
                    rule="compile_cpp",
                    inputs=n,
                    implicit=deps,
                    variables=kwargs)
        src_objs.append(obj_name)
    ninja.build(bin_name, "link", src_objs)


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
                inputs="/usr/share/verilator/include/verilated.cpp",
                outputs="obj/verilated.o")

# ------------------------------------------------------------------------------


treesitter_objs = []


def build_treesitter():
    divider("TreeSitter libraries")

    treesitter_srcs = [
        "submodules/tree-sitter/lib/src/lib.c",
        "submodules/tree-sitter-cpp/src/parser.c",
        "submodules/tree-sitter-cpp/src/scanner.cc",
    ]

    for n in treesitter_srcs:
        o = path.join(obj_dir, swap_ext(n, ".o"))
        ninja.build(
            o,
            "compile_c",
            n,
            includes="-Isubmodules/tree-sitter/lib/include"
        )
        treesitter_objs.append(o)

# ------------------------------------------------------------------------------


def build_metron():
    cpp_binary(
        bin_name="bin/metron",
        src_files=[
            "src/MetronApp.cpp",
            "src/MtCursor.cpp",
            "src/MtModLibrary.cpp",
            "src/MtModule.cpp",
            "src/MtNode.cpp",
            "src/MtSourceFile.cpp",
            "src/MtTracer.cpp",
            "src/Platform.cpp",
        ],
        includes=[
            ".",
            "submodules/tree-sitter/lib/include"
        ],
        src_objs=treesitter_objs,
    )

# ------------------------------------------------------------------------------
# Simple Uart testbench

# FIXME move rules out of this block


def build_uart():
    cpp_binary(
        bin_name="bin/examples/uart",
        src_files=["examples/uart/main.cpp"],
        includes=["src"],
        # FIXME Why the F does the build break if I don't pass an empty array here?
        src_objs=[],
    )

    uart_srcs = metronize_dir("examples/uart/metron",
                              "examples/uart/metron_sv")

    uart_vhdr, uart_vobj = verilate_dir(
        src_dir="examples/uart/metron_sv",
        src_files=uart_srcs,
        src_top="uart_top",
        dst_dir="examples/uart/metron_vl",
    )

    cpp_binary(
        bin_name="bin/examples/uart_vl",
        src_files=["examples/uart/main_vl.cpp"],
        includes=[
            "src",
            "tests",
            "obj/examples/uart",
            "/usr/local/share/verilator/include"
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

    iverilog_binary(
        bin_name="bin/examples/uart_iv",
        src_top="examples/uart/uart_test_iv.sv",
        src_files=uart_srcs,
        includes=uart_includes)

    divider("Yosys/NextPNR uart testbench")

    ninja.build(rule="yosys",
                inputs="examples/uart/uart_test_ice40.sv",
                implicit=uart_srcs,
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
    src_files = glob.glob("tests/rv_tests/*.S")
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
    vl_root = "examples/rvsimple/metron_vl"

    cpp_binary(
        bin_name="bin/examples/rvsimple",
        src_files=["examples/rvsimple/main.cpp"],
        includes=[
            ".",
            "src"
        ],
        opt="-O3",
    )

    rvsimple_metron_srcs = metronize_dir(mt_root, sv_root)

    rvsimple_metron_vhdr, rvsimple_metron_vobj = verilate_dir(
        src_dir=sv_root,
        src_files=rvsimple_metron_srcs,
        src_top="toplevel",
        dst_dir=vl_root,
    )

    cpp_binary(
        bin_name="bin/examples/rvsimple_vl",
        src_files=["examples/rvsimple/main_vl.cpp"],
        includes=[
            ".",
            "src",
            "tests",
            "obj/examples/rvsimple",
            "/usr/local/share/verilator/include"
        ],
        src_objs=["obj/verilated.o", rvsimple_metron_vobj],
        deps=[rvsimple_metron_vhdr]
    )

    rvsimple_reference_vhdr, rvsimple_reference_vobj = verilate_dir(
        src_dir="examples/rvsimple/reference",
        src_files=glob.glob("examples/rvsimple/reference/*.sv"),
        src_top="toplevel",
        dst_dir="examples/rvsimple/reference_vl"
    )

    cpp_binary(
        bin_name="bin/examples/rvsimple_ref",
        src_files=["examples/rvsimple/main_ref_vl.cpp"],
        includes=[
            ".",
            "src",
            "tests",
            "obj/examples/rvsimple",
            "/usr/local/share/verilator/include"
        ],
        src_objs=["obj/verilated.o", rvsimple_reference_vobj],
        deps=[rvsimple_reference_vhdr]
    )


# ------------------------------------------------------------------------------
# RVTiny

def build_rvtiny():
    mt_root = "examples/rvtiny/metron"
    sv_root = "examples/rvtiny/metron_sv"
    vl_root = "examples/rvtiny/metron_vl"

    cpp_binary(
        bin_name="bin/examples/rvtiny",
        src_files=["examples/rvtiny/main.cpp"],
        includes=[
            ".",
            "src"
        ],
        opt="-O3",
    )

    rvtiny_metron_srcs = metronize_dir(mt_root, sv_root)

    rvtiny_metron_vhdr, rvtiny_metron_vobj = verilate_dir(
        src_dir=sv_root,
        src_files=rvtiny_metron_srcs,
        src_top="toplevel",
        dst_dir=vl_root
    )

    cpp_binary(
        bin_name="bin/examples/rvtiny_vl",
        src_files=["examples/rvtiny/main_vl.cpp"],
        includes=[
            ".",
            "src",
            "obj/examples/rvtiny",
            "/usr/local/share/verilator/include"
        ],
        src_objs=["obj/verilated.o", rvtiny_metron_vobj],
        deps=[rvtiny_metron_vhdr]
    )


# ------------------------------------------------------------------------------
# RVTiny_Sync - synchronous-mem-only version of RVTiny

def build_rvtiny_sync():
    mt_root = "examples/rvtiny_sync/metron"
    sv_root = "examples/rvtiny_sync/metron_sv"
    vl_root = "examples/rvtiny_sync/metron_vl"

    cpp_binary(
        bin_name="bin/examples/rvtiny_sync",
        src_files=["examples/rvtiny_sync/main.cpp"],
        includes=[
            ".",
            "src"
        ],
        opt="-O3",
    )

    metronized_src = metronize_dir(mt_root, sv_root)

    verilated_h, verilated_o = verilate_dir(
        src_dir=sv_root,
        src_files=metronized_src,
        src_top="toplevel",
        dst_dir=vl_root
    )

    cpp_binary(
        bin_name="bin/examples/rvtiny_sync_vl",
        src_files=["examples/rvtiny_sync/main_vl.cpp"],
        includes=[
            ".",
            "src",
            "/usr/local/share/verilator/include"
        ],
        src_objs=["obj/verilated.o", verilated_o],
        deps=[verilated_h]
    )

# ------------------------------------------------------------------------------
# Ibex

def build_ibex():
    cpp_binary(
        bin_name="bin/examples/ibex",
        src_files=["examples/ibex/main.cpp"],
        includes=[
            ".",
            "src"
        ],
        opt="-O3",
    )

# ------------------------------------------------------------------------------
# Pong

def build_pong():
    mt_root = "examples/pong/metron"
    sv_root = "examples/pong/metron_sv"
    vl_root = "examples/pong/metron_vl"

    cpp_binary(
        bin_name="bin/examples/pong",
        src_files=["examples/pong/main.cpp"],
        includes=[
            ".",
            "src"
        ],
        opt="-O3",
    )

    metronized_src = metronize_dir(mt_root, sv_root)

    """
    rvtiny_sync_metron_vl_h, rvtiny_sync_metron_vl_o = verilate_dir(
        src_dir="examples/rvtiny_sync/metron_sv",
        src_files=rvtiny_sync_metron_sv,
        src_top="toplevel",
        dst_dir="examples/rvtiny_sync/metron_vl"
    )

    cpp_binary(
        bin_name="bin/examples/rvtiny_sync_vl",
        src_files=["examples/rvtiny_sync/main_vl.cpp"],
        includes=[
            ".",
            "src",
            "obj/examples/rvtiny_sync",
            "/usr/local/share/verilator/include"
        ],
        src_objs=["obj/verilated.o", rvtiny_sync_metron_vl_o],
        deps=[rvtiny_sync_metron_vl_h]
    )
    """


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
    main()
