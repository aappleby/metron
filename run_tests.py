#!/usr/bin/env python3
import os
import sys
import glob
import subprocess
import multiprocessing
import shlex
import argparse

parser = argparse.ArgumentParser()
parser.add_argument(
    "--basic", action="store_true", help="Test only basic functionality"
)
parser.add_argument("--verbose", action="store_true", help="Print all commands tested")
parser.add_argument(
    "--serial", action="store_true", help="Run only one command at a time"
)
parser.add_argument(
    "--synth", action="store_true", help="Test Yosys synthesis (slower)"
)
parser.add_argument(
    "--coverage", action="store_true", help="Run Metron under kcov for coverage testing"
)
options = parser.parse_args()


def sorted_glob(*args, **kwargs):
    return sorted(glob.glob(*args, **kwargs))


################################################################################


def main():
    print()
    print_b(" ###    ### ####### ######## ######   ######  ###    ## ")
    print_b(" ####  #### ##         ##    ##   ## ##    ## ####   ## ")
    print_b(" ## #### ## #####      ##    ######  ##    ## ## ##  ## ")
    print_b(" ##  ##  ## ##         ##    ##   ## ##    ## ##  ## ## ")
    print_b(" ##      ## #######    ##    ##   ##  ######  ##   #### ")

    ############################################################

    errors = 0

    print()
    print_b("Refreshing build")
    os.system("./build.py")

    if options.basic:
        if os.system("ninja bin/metron"):
            print("Build failed!")
            return -1
    else:
        if os.system("ninja"):
            print("Build failed!")
            return -1

    if options.coverage:
        print("Wiping old coverage run")
        os.system("rm -rf coverage")

    ############################################################


    print_b("Wiping tests/metron_sv/*")
    os.system("rm tests/metron_sv/*")

    print_b("Checking that examples convert to SV cleanly")
    errors += check_commands_good(
        [
            f"bin/metron -c examples/uart/metron/uart_top.h",
            f"bin/metron -c examples/rvsimple/metron/toplevel.h",
            f"bin/metron -c examples/pong/metron/pong.h",
        ]
    )
    print()

    metron_good = sorted(sorted_glob("tests/metron_good/*.h"))
    metron_bad = sorted(sorted_glob("tests/metron_bad/*.h"))

    print_b("Checking that all headers in tests/metron_good compile")
    errors += check_commands_good(
        [
            f"g++ -I. --std=gnu++2a -fsyntax-only -c {filename}"
            for filename in metron_good
        ]
    )
    print()

    print_b("Checking that all headers in tests/metron_bad compile")
    errors += check_commands_good(
        [
            f"g++ -I. --std=gnu++2a -fsyntax-only -c {filename}"
            for filename in metron_bad
        ]
    )
    print()

    print_b("Checking that all test cases in metron_good convert to SV cleanly")
    errors += check_commands_good(
        [
            f"bin/metron -c {filename} -o {filename.replace('_good', '_sv').replace('.h', '.sv')}"
            for filename in metron_good
        ]
    )
    print()

    print_b("Checking that all test cases in metron_bad fail conversion")
    errors += check_commands_bad(
        [
            f"bin/metron -c {filename} -o {filename.replace('_bad', '_sv').replace('.h', '.sv')}"
            for filename in metron_bad
        ]
    )
    print()

    metron_sv = sorted(sorted_glob("tests/metron_sv/*.sv"))

    print_b("Checking that all converted files match their golden version, if present")
    errors += check_commands_good(
        [
            f"diff {filename} {filename.replace('_sv', '_golden')}"
            for filename in metron_sv
        ]
    )
    print()

    ################################################################################
    # These tests are skipped in basic mode

    if not options.basic:
        print_b("Checking that all converted files can be parsed by Verilator")
        errors += check_commands_good(
            [
                f"verilator -Wno-width -I. --lint-only {filename}"
                for filename in metron_sv
            ]
        )
        print()

        if options.synth:
            print_b("Checking that all converted files can be synthesized by Yosys")
            errors += check_commands_good(
                [
                    f"yosys -q -p 'read_verilog -I. -sv {filename}; dump; synth_ice40 -json /dev/null'"
                    for filename in metron_sv
                ]
            )
        else:
            print_b("Checking that all converted files can be parsed by Yosys")
            errors += check_commands_good(
                [
                    f"yosys -q -p 'read_verilog -I. -sv {filename};'"
                    for filename in metron_sv
                ]
            )
        print()

        print_b("Checking that all converted files can be parsed by Icarus")
        errors += check_commands_good(
            [
                f"iverilog -g2012 -Wall -I. -o /dev/null {filename}"
                for filename in metron_sv
            ]
        )
        print()

        print_b("Running misc bad commands")
        errors += check_commands_bad(
            [
                f"bin/metron skjdlsfjkhdfsjhdf.h",
                f"bin/metron -c skjdlsfjkhdfsjhdf.h",
            ]
        )
        print()

        print_b("Running standalone tests")
        errors += check_commands_good(
            [
                "bin/tests/metron_test",
                "bin/examples/uart",
                "bin/examples/uart_vl",
                "bin/examples/uart_iv",
                "bin/examples/rvsimple",
                "bin/examples/rvsimple_vl",
                "bin/examples/rvsimple_ref",
            ]
        )
        print()

        # Lockstep tests are slow because compiler...
        print_b("Testing lockstep simulations")
        errors += test_lockstep()
        print()

        # Various tests to isolate quirks/bugs in Verilator/Yosys/Icarus

        tools_good = sorted_glob("tests/tools/good/*.sv")
        verilator_bad = sorted_glob("tests/tools/verilator/*.sv")
        yosys_bad = sorted_glob("tests/tools/yosys_fail/*.sv")
        icarus_bad = sorted_glob("tests/tools/icarus/*.sv")

        print_b("Checking Verilator quirks")
        errors += check_verilator_good(tools_good)
        errors += check_verilator_bad(verilator_bad)
        print()

        print_b("Checking Yosys quirks")
        errors += check_yosys_good(tools_good)
        errors += check_yosys_bad(yosys_bad)
        print()

        print_b("Checking Icarus quirks")
        errors += check_icarus_good(tools_good)
        errors += check_icarus_bad(icarus_bad)
        print()

    ############################################################

    print()
    print_b(f"Total failures : {errors}")
    print()
    if errors > 0:
        print_r(" #######  #####  ## ##      ")
        print_r(" ##      ##   ## ## ##      ")
        print_r(" #####   ####### ## ##      ")
        print_r(" ##      ##   ## ## ##      ")
        print_r(" ##      ##   ## ## ####### ")
    else:
        print_g(" ######   #####  ####### ####### ")
        print_g(" ##   ## ##   ## ##      ##      ")
        print_g(" ######  ####### ####### ####### ")
        print_g(" ##      ##   ##      ##      ## ")
        print_g(" ##      ##   ## ####### ####### ")
    return errors


################################################################################


def get_pool():
    max_threads = multiprocessing.cpu_count()
    if options.coverage:
        max_threads = 1
    if options.serial:
        max_threads = 1
    return multiprocessing.Pool(max_threads)


def print_c(color, *args):
    sys.stdout.write(
        f"\u001b[38;2;{(color >> 16) & 0xFF};{(color >> 8) & 0xFF};{(color >> 0) & 0xFF}m"
    )
    print(*args)
    sys.stdout.write("\u001b[0m")
    sys.stdout.flush()


def print_r(*args):
    print_c(0xFF8080, *args)


def print_g(*args):
    print_c(0x80FF80, *args)


def print_b(*args):
    print_c(0x8080FF, *args)


################################################################################


def prep_cmd(cmd):
    cmd = cmd.strip()
    kcov_prefix = "kcov --exclude-region=KCOV_OFF:KCOV_ON --include-pattern=metron --exclude-pattern=submodules --exclude-line=debugbreak coverage"
    if options.coverage and cmd.startswith("bin/metron "):
        cmd = kcov_prefix + " " + cmd
    args = [arg for arg in shlex.split(cmd) if len(arg)]
    return args


def check_cmd_good(cmd):
    if options.verbose:
        print(cmd)
    else:
        print(".", end="")
    sys.stdout.flush()
    prepped = prep_cmd(cmd)
    cmd_result = subprocess.run(
        prepped, stdout=subprocess.PIPE, stderr=subprocess.PIPE, encoding="charmap"
    )
    if cmd_result.returncode:
        print()
        print_r(f"Command failed: {cmd}")
        return 1
    return 0


def check_cmd_bad(cmd, expected_outputs=[], expected_errors=[]):
    if options.verbose:
        print(cmd)
    else:
        print(".", end="")
    sys.stdout.flush()
    cmd_result = subprocess.run(
        prep_cmd(cmd),
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        encoding="charmap",
    )
    if cmd_result.returncode == 0:
        print()
        print_r(f"Command passed: {cmd}")
        return 1
    elif cmd_result.returncode == 34048:
        print()
        print_r(f"Command threw an exception: {cmd}")
        return 1
    else:
        for text in expected_outputs:
            if not text in cmd_result.stout:
                print()
                print_r(f"Command {cmd}")
                print_r(f'Did not produce expected output "{text}"')
                return 1
        for err in expected_errors:
            if not err in cmd_result.stout:
                print()
                print_r(f"Command {cmd}")
                print_r(f'Did not produce expected error "{err}"')
                return 1
        return 0


def check_commands_good(commands):
    result = sum(get_pool().map(check_cmd_good, commands))
    return result


def check_commands_bad(commands):
    result = sum(get_pool().map(check_cmd_bad, commands))
    return result


###############################################################################


def check_verilator_good(filenames):
    return check_commands_good(
        [f"verilator -Wno-width -I. --lint-only {filename}" for filename in filenames]
    )


def check_verilator_bad(filenames):
    return check_commands_bad(
        [f"verilator -Wno-width -I. --lint-only {filename}" for filename in filenames]
    )


def check_yosys_good(filenames):
    for filename in filenames:
        cmd = f"yosys -q -p 'read_verilog -I. -sv {filename};  dump; synth_ice40 -json /dev/null'"
        if options.verbose:
            print(cmd)
        else:
            print(".", end="")
        sys.stdout.flush()
        cmd_result = subprocess.run(
            prep_cmd(cmd),
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            encoding="charmap",
        )
        err = cmd_result.stderr.lower()
        if "warning" in err or "error" in err:
            print(f"Printed 'warning' or 'error' for {filename}")
            print(cmd_result.stderr)
            return 1
        else:
            return 0


def check_yosys_bad(filenames):
    for filename in filenames:
        cmd = f"yosys -q -p 'read_verilog -I. -sv {filename};  dump; synth_ice40 -json /dev/null'"
        if options.verbose:
            print(cmd)
        else:
            print(".", end="")
        sys.stdout.flush()
        cmd_result = subprocess.run(
            prep_cmd(cmd),
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            encoding="charmap",
        )
        err = cmd_result.stderr.lower()
        if "warning" in err or "error" in err:
            return 0
        else:
            print(f"Did not print 'warning' or 'error' for {filename}")
            return 1


def check_icarus_good(filenames):
    return check_commands_good(
        [f"iverilog -g2012 -Wall -I. -o /dev/null {filename}" for filename in filenames]
    )


def check_icarus_bad(filenames):
    return check_commands_bad(
        [f"iverilog -g2012 -Wall -I. -o /dev/null {filename}" for filename in filenames]
    )


###############################################################################
# Make sure all lines starting with "// X " in the source text appear in the
# output


def check_bad_expected_errors(filename):
    lines = open(filename).readlines()
    expected_errors = [line[4:].strip() for line in lines if line.startswith("// X ")]
    cmd = f"bin/metron -c {filename} -o {filename.replace('_good', '_sv').replace('.h', '.sv')}"
    return check_cmd_bad(cmd, expected_errors, [])


################################################################################


def build_lockstep(filename):
    test_name = filename.rstrip(".h")

    # Test source is the same for all lockstep tests, we just change the
    # included files.
    test_src = f"tests/test_lockstep.cpp"

    mt_root = f"tests/metron_lockstep"
    sv_root = f"gen/{mt_root}/metron_sv"
    vl_root = f"gen/{mt_root}/metron_vl"

    # Our lockstep test top modules are all named "Module". Verilator will
    # name the top module after the <test_name>.sv filename.
    mt_top = f"Module"
    vl_top = f"V{test_name}"

    mt_header = f"{mt_root}/{test_name}.h"
    vl_header = f"{vl_root}/V{test_name}.h"
    vl_obj = f"{vl_root}/V{test_name}__ALL.o"
    test_obj = f"obj/{mt_root}/{test_name}.o"
    test_bin = f"bin/{mt_root}/{test_name}"

    includes = f"-I. -Isymlinks -Isymlinks/metrolib -I{sv_root} -I/usr/local/share/verilator/include"

    errors = 0

    cmd = f"bin/metron -q -c {mt_root}/{test_name}.h -o {sv_root}/{test_name}.sv"
    errors += check_cmd_good(cmd)

    cmd = f"verilator -Wno-width {includes} --cc {test_name}.sv -Mdir {vl_root}"
    errors += check_cmd_good(cmd)

    cmd = f"make -C {vl_root} -f V{test_name}.mk"
    errors += check_cmd_good(cmd)

    cmd = f"g++ -O3 -std=gnu++2a -DMT_TOP={mt_top} -DVL_TOP={vl_top} -DMT_HEADER={mt_header} -DVL_HEADER={vl_header} {includes} -c {test_src} -o {test_obj}"
    errors += check_cmd_good(cmd)

    cmd = f"g++ {test_obj} {vl_obj} symlinks/metrolib/obj/metrolib/core/Utils.o obj/verilated.o obj/verilated_threads.o -o {test_bin}"
    errors += check_cmd_good(cmd)

    return errors


################################################################################


def test_lockstep():
    tests = [
        "counter.h",
        "lfsr.h",
        "funcs_and_tasks.h",
        "lockstep_bad.h",
        "timeout_bad.h",
    ]

    os.system(f"mkdir -p gen/tests/metron_lockstep")
    os.system(f"mkdir -p obj/tests/metron_lockstep")
    os.system(f"mkdir -p bin/tests/metron_lockstep")

    # Build all the lockstep tests
    errors = 0
    errors += sum(get_pool().map(build_lockstep, tests))

    # These lockstep tests should pass
    errors += check_cmd_good("bin/tests/metron_lockstep/counter")
    errors += check_cmd_good("bin/tests/metron_lockstep/lfsr")
    errors += check_cmd_good("bin/tests/metron_lockstep/funcs_and_tasks")

    # These two are expected to fail to test the lockstep test system
    errors += check_cmd_bad("bin/tests/metron_lockstep/lockstep_bad")
    errors += check_cmd_bad("bin/tests/metron_lockstep/timeout_bad")

    return errors


################################################################################


if __name__ == "__main__":
    sys.exit(main())
