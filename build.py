#!/usr/bin/env python3

import glob
import ninja_syntax
import sys
import os
from os import path


obj_dir = "obj"
outfile = open("build.ninja", "w+")
ninja = ninja_syntax.Writer(outfile)
#opt_mode = "-O3"
opt_mode = ""

base_includes = [
    ".",
    "src",
    "tests",
    "/usr/local/share/verilator/include",
]

# ------------------------------------------------------------------------------


def main():
    global outfile
    print("Regenerating build.ninja...")
    build_verilator()
    build_treesitter()
    build_metron_lib()
    build_metron_app()
    build_metron_test()
    build_rvtests()
    build_uart()
    build_rvsimple()
    build_rvtiny()
    build_rvtiny_sync()
    # build_ibex()
    build_pong()
    print("Done!")
    outfile.close()
    outfile = None

    # this hangs...
    # return os.system("ninja")
    return 0

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
           command="g++ -rdynamic -g ${opt} -std=gnu++2a ${includes} -MMD -MF ${out}.d -c ${in} -o ${out}", deps="gcc", depfile="${out}.d")
ninja.rule("compile_c",
           command="gcc -rdynamic -g ${opt} ${includes} -MMD -MF ${out}.d -c ${in} -o ${out}", deps="gcc", depfile="${out}.d")
ninja.rule("static_lib",    command="ar rcs ${out} ${in} > /dev/null")
ninja.rule("link",
           command="g++ -rdynamic -g $opt ${in} -Wl,--whole-archive ${local_libs} -Wl,--no-whole-archive ${global_libs} -o ${out}")

# yes, we run metron with quiet and verbose both on for test coverage
ninja.rule("metron",
           command="bin/metron -q -v -r ${src_dir} -o ${dst_dir} -c ${src_top}")

ninja.rule("verilator",
           command="verilator ${includes} --cc ${src_top} -Mdir ${dst_dir}")
ninja.rule("make",
           command="make --quiet -C ${dst_dir} -f ${makefile} > /dev/null")
ninja.rule("run_test",
           command="${in} | grep \"All tests pass.\" && touch ${out}")
ninja.rule("console",       command="${in}", pool="console")
ninja.rule("iverilog",
           command="iverilog -g2012 ${defines} ${includes} ${in} -o ${out}")
ninja.rule("yosys",
           command="yosys -q -p 'read_verilog ${includes} -sv ${in}; dump; synth_ice40 -json ${out};'")
ninja.rule("nextpnr-ice40",
           command="nextpnr-ice40 -q --${chip} --package ${package} --json ${in} --asc ${out} --pcf ${pcf}")
ninja.rule("icepack",       command="icepack ${in} ${out}")


def metronize_dir(src_dir, src_top, dst_dir):
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
                implicit=["bin/metron"] + src_paths,
                outputs=dst_paths,
                src_dir=src_dir,
                dst_dir=dst_dir,
                src_top=src_top)

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
    ninja.build(outputs=bin_name,
                rule="link",
                inputs=src_objs + deps,
                variables=kwargs)


def cpp_library(lib_name, src_files, src_objs=None, deps=None, **kwargs):
    """
    Compiles a C++ binary from the given source files.
    """
    if deps is None:
        deps = []

    divider(f"Create static library {lib_name}")

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
    ninja.build(outputs=lib_name,
                rule="static_lib",
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
# Build Metron itself


def build_metron_lib():
    cpp_library(
        lib_name="bin/libmetron.a",
        src_files=[
            "src/Err.cpp",
            "src/MtChecker.cpp",
            "src/MtContext.cpp",
            "src/MtCursor.cpp",
            "src/MtField.cpp",
            "src/MtFuncParam.cpp",
            "src/MtMethod.cpp",
            "src/MtModLibrary.cpp",
            "src/MtModParam.cpp",
            "src/MtModule.cpp",
            "src/MtNode.cpp",
            "src/MtSourceFile.cpp",
            "src/MtTracer.cpp",
            "src/MtUtils.cpp",
            "src/Platform.cpp",
        ],
        includes=[
            ".",
            "submodules/tree-sitter/lib/include"
        ],
        src_objs=treesitter_objs,
    )

# ------------------------------------------------------------------------------


def build_metron_app():
    cpp_binary(
        bin_name="bin/metron",
        src_files=[
            "src/MetronApp.cpp",
        ],
        includes=[
            ".",
            "bin",
            "submodules/tree-sitter/lib/include"
        ],
        deps=["bin/libmetron.a"],
    )

# ------------------------------------------------------------------------------
# Low-level tests


def build_metron_test():
    cpp_binary(
        bin_name="bin/metron_test",
        src_files=[
            "tests/test_main.cpp",
            "tests/test_logic.cpp",
            "tests/test_utils.cpp",
        ],
        includes=base_includes,
        deps=["bin/libmetron.a"],
    )

# ------------------------------------------------------------------------------
# Simple Uart testbench


def build_uart():
    cpp_binary(
        bin_name="bin/examples/uart",
        src_files=["examples/uart/main.cpp"],
        includes=["src"],
        # FIXME Why the F does the build break if I don't pass an empty array here?
        src_objs=[],
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
        includes=base_includes + ["gen/examples/uart"],
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
    vl_root = "gen/examples/rvsimple/metron_vl"

    cpp_binary(
        bin_name="bin/examples/rvsimple",
        src_files=["examples/rvsimple/main.cpp"],
        includes=base_includes,
        opt=opt_mode,
        deps=["bin/libmetron.a"],
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
        includes=base_includes + [vl_root],
        src_objs=["obj/verilated.o", vl_vobj],
        deps=[vl_vhdr]
    )

    ref_sv_root = "examples/rvsimple/reference_sv"
    ref_vl_root = "gen/examples/rvsimple/reference_vl"

    ref_vhdr, ref_vobj = verilate_dir(
        src_dir=ref_sv_root,
        src_files=glob.glob(f"{ref_sv_root}/*.sv"),
        src_top="toplevel",
        dst_dir=ref_vl_root
    )

    cpp_binary(
        bin_name="bin/examples/rvsimple_ref",
        src_files=["examples/rvsimple/main_ref_vl.cpp"],
        includes=base_includes + [ref_vl_root],
        src_objs=["obj/verilated.o", ref_vobj],
        deps=[ref_vhdr]
    )


# ------------------------------------------------------------------------------
# RVTiny

def build_rvtiny():
    mt_root = "examples/rvtiny/metron"
    sv_root = "examples/rvtiny/metron_sv"
    vl_root = "gen/examples/rvtiny/metron_vl"

    cpp_binary(
        bin_name="bin/examples/rvtiny",
        src_files=["examples/rvtiny/main.cpp"],
        includes=base_includes + [mt_root],
        deps=["bin/libmetron.a"],
        opt=opt_mode,
    )

    rvtiny_sv_srcs = metronize_dir(mt_root, "toplevel.h", sv_root)

    rvtiny_vl_vhdr, rvtiny_vl_vobj = verilate_dir(
        src_dir=sv_root,
        src_files=rvtiny_sv_srcs,
        src_top="toplevel",
        dst_dir=vl_root
    )

    cpp_binary(
        bin_name="bin/examples/rvtiny_vl",
        src_files=["examples/rvtiny/main_vl.cpp"],
        includes=base_includes + [vl_root],
        src_objs=["obj/verilated.o", rvtiny_vl_vobj],
        deps=[rvtiny_vl_vhdr]
    )


# ------------------------------------------------------------------------------
# RVTiny_Sync - synchronous-mem-only version of RVTiny

def build_rvtiny_sync():
    mt_root = "examples/rvtiny_sync/metron"
    sv_root = "examples/rvtiny_sync/metron_sv"
    vl_root = "gen/examples/rvtiny_sync/metron_vl"

    cpp_binary(
        bin_name="bin/examples/rvtiny_sync",
        src_files=["examples/rvtiny_sync/main.cpp"],
        includes=base_includes,
        deps=["bin/libmetron.a"],
        opt=opt_mode,
    )

    metronized_src = metronize_dir(mt_root, "toplevel.h", sv_root)

    verilated_h, verilated_o = verilate_dir(
        src_dir=sv_root,
        src_files=metronized_src,
        src_top="toplevel",
        dst_dir=vl_root
    )

    cpp_binary(
        bin_name="bin/examples/rvtiny_sync_vl",
        src_files=["examples/rvtiny_sync/main_vl.cpp"],
        includes=base_includes + [vl_root],
        src_objs=["obj/verilated.o", verilated_o],
        deps=[verilated_h]
    )

# ------------------------------------------------------------------------------
# Ibex


def build_ibex():
    cpp_binary(
        bin_name="bin/examples/ibex",
        src_files=["examples/ibex/main.cpp"],
        includes=base_includes,
        opt=opt_mode,
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
        includes=base_includes,
        global_libs="-lSDL2",
        opt=opt_mode,
        deps=["bin/libmetron.a"],
    )

    metronized_src = metronize_dir(mt_root, "pong.h", sv_root)

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
    sys.exit(main())
