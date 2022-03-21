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

divider("Rules")

ninja.rule(name="compile_cpp",
           command="g++ -g $opt -std=gnu++2a ${includes} -MMD -MF ${out}.d -c ${in} -o ${out}",
           deps="gcc",
           depfile="${out}.d")

ninja.rule(name="compile_c",
           command="gcc -g $opt ${includes} -MMD -MF ${out}.d -c ${in} -o ${out}",
           deps="gcc",
           depfile="${out}.d")

ninja.rule(name="link", command="g++ -g $opt ${in} ${link_libs} -o ${out}")
# command = "g++ -g $opt -Wl,--whole-archive ${in} -Wl,--no-whole-archive ${link_libs} -o ${out}"

ninja.rule(name="iverilog",
           command="iverilog -g2012 -DIVERILOG ${in} -o ${out}")

ninja.rule(name="yosys",
           command="yosys -p 'read_verilog -sv ${in}; dump; synth_ice40 -json ${out};'")

ninja.rule(name="nextpnr-ice40",
           command="nextpnr-ice40 -q --hx8k --package ct256 --json ${in} --asc ${out} --pcf ${pcf}")

ninja.rule(name="iceprog", command="icepack ${in} ${out} && iceprog -S ${out}")

ninja.rule(name="run_test",
           command="${in} | grep \"All tests pass.\" && touch ${out}")

ninja.rule(name="sv2v", command="sv2v -w ${out} ${in}")

ninja.rule(name="run_in_console", command="${in}", pool="console")

ninja.rule(name="run_cmd", command="${cmd}")

# ------------------------------------------------------------------------------


def run_cmd(in_files, cmd, out_files):
    ninja.build(
        out_files,
        "run_cmd",
        in_files,
        variables={"cmd": cmd}
    )

# ------------------------------------------------------------------------------


def metronate_dir(src_dir, dst_dir):
    """
    Convert all .h files in the source directory into .sv files in the destination directory.
    Returns an array of converted filenames.
    """
    divider(f"Metronate {src_dir} -> {dst_dir}")
    src_paths = glob.glob(path.join(src_dir, "*.h"))
    src_files = [path.basename(n) for n in src_paths]
    dst_paths = [path.join(dst_dir, swap_ext(n, ".sv")) for n in src_files]
    run_cmd(in_files=["bin/metron"] + src_paths,
            cmd=f"bin/metron -q -R{src_dir} -O{dst_dir} {' '.join(src_files)}",
            out_files=dst_paths)

    return dst_paths

# ------------------------------------------------------------------------------


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
    run_cmd(in_files=glob.glob(f"{src_dir}/*.sv"),
            cmd=f"verilator -I{src_dir} -Isrc --cc {src_top} -Mdir {dst_dir}",
            out_files=[verilated_make, verilated_hdr])

    # Compile via makefile to generate object file
    run_cmd(in_files=verilated_make,
            cmd=f"make -C {dst_dir} -f V{src_top}.mk",
            out_files=verilated_obj)

    return (verilated_hdr, verilated_obj)

# ------------------------------------------------------------------------------


def cpp_binary(bin_name, src_files, includes, src_objs = [], variables = {}, deps = []):
    """
    Compiles a C++ binary from the given source files.
    """
    divider(f"Compile {bin_name}")
    variables["includes"] = includes
    for n in src_files:
        obj_name = path.join(obj_dir, swap_ext(n, ".o"))
        ninja.build(obj_name, "compile_cpp", n,
                    implicit=deps, variables=variables)
        src_objs.append(obj_name)
    ninja.build(bin_name, "link", src_objs)


# ------------------------------------------------------------------------------
divider("Verilator libraries")

ninja.build(
    "obj/verilated.o",
    "compile_cpp",
    "/usr/share/verilator/include/verilated.cpp",
    variables={
        "includes": ""
    }
)

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
        variables={
            "includes": "-Itree-sitter/lib/include"
        }
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

cpp_binary(
    bin_name="bin/example_uart/rtl/main",
    src_files=["example_uart/rtl/main.cpp"],
    includes=["-Isrc"],
    # FIXME Why the F does the build break if I don't pass an empty array here?
    src_objs=[],
)

# ------------------------------------------------------------------------------

metronate_dir("example_uart/rtl", "example_uart/generated")

uart_generated_hdr, uart_generated_obj = verilate_dir(
    src_dir="example_uart/generated",
    src_top="uart_top",
    dst_dir=path.join(obj_dir, "example_uart/generated")
)

cpp_binary(
    bin_name="bin/example_uart/generated/main",
    src_files=["example_uart/generated/main.cpp"],
    includes=[
        "-Isrc",
        "-Itests",
        "-Iobj/example_uart/generated",
        "-I/usr/local/share/verilator/include"
    ],
    src_objs=["obj/verilated.o", uart_generated_obj],
    deps=[uart_generated_hdr]
)

# ------------------------------------------------------------------------------

cpp_binary(
    bin_name="bin/example_rvsimple/rtl/main",
    src_files=["example_rvsimple/rtl/main.cpp"],
    includes=["-Isrc"],
    variables={"opt": "-O3"},
)

# ------------------------------------------------------------------------------

metronate_dir("example_rvsimple/rtl", "example_rvsimple/generated")

rvsimple_generated_hdr, rvsimple_generated_obj = verilate_dir(
    src_dir="example_rvsimple/generated",
    src_top="toplevel",
    dst_dir=path.join(obj_dir, "example_rvsimple/generated")
)

cpp_binary(
    bin_name="bin/example_rvsimple/generated/main",
    src_files=["example_rvsimple/generated/main.cpp"],
    includes=[
        "-Isrc",
        "-Itests",
        "-Iobj/example_rvsimple/generated",
        "-I/usr/local/share/verilator/include"
    ],
    src_objs=["obj/verilated.o", rvsimple_generated_obj],
    deps=[rvsimple_generated_hdr]
)

# ------------------------------------------------------------------------------

rvsimple_reference_hdr, rvsimple_reference_obj = verilate_dir(
    src_dir="example_rvsimple/reference",
    src_top="toplevel",
    dst_dir=path.join(obj_dir, "example_rvsimple/reference")
)

cpp_binary(
    bin_name="bin/example_rvsimple/reference/main",
    src_files=["example_rvsimple/reference/main.cpp"],
    includes=[
        "-Isrc",
        "-Itests",
        "-Iobj/example_rvsimple/reference",
        "-I/usr/local/share/verilator/include"
    ],
    src_objs=["obj/verilated.o", rvsimple_reference_obj],
    deps=[rvsimple_reference_hdr]
)

# ------------------------------------------------------------------------------

divider("Done!")


""""
# build benchmark_reference : run_always bin/example_rvsimple/main_reference
# build benchmark_generated : run_always bin/example_rvsimple/main_generated
# build benchmark_rtl : run_always bin/example_rvsimple/main_rtl

# build benchmark : phony benchmark_reference benchmark_generated benchmark_rtl

# default bin/metron
# default bin/example_rvsimple/main_reference
# default bin/example_rvsimple/main_generated
# default bin/example_rvsimple/main_rtl
"""
