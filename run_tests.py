#!/usr/bin/env python3
import os
import sys
import glob
import subprocess
import multiprocessing
import time
from os import path

################################################################################


def main():
    print()
    print_b(" ###    ### ####### ######## ######   ######  ###    ## ")
    print_b(" ####  #### ##         ##    ##   ## ##    ## ####   ## ")
    print_b(" ## #### ## #####      ##    ######  ##    ## ## ##  ## ")
    print_b(" ##  ##  ## ##         ##    ##   ## ##    ## ##  ## ## ")
    print_b(" ##      ## #######    ##    ##   ##  ######  ##   #### ")

    basic = "--basic" in sys.argv

    ############################################################

    print()
    print_b("Refreshing build")

    if basic:
        if os.system("ninja bin/metron"):
            print("Build failed!")
            sys.exit(-1)
    else:
        if os.system("ninja"):
            print("Build failed!")
            sys.exit(-1)

    if "--coverage" in sys.argv:
        print("Wiping old coverage run")
        os.system("rm -rf coverage")

    ############################################################

    errors = 0

    """
    print("verilator");
    os.system("verilator -Isrc --lint-only tests/feature_test.sv")

    print("iverilog");
    os.system("iverilog -g2012 -Wall -Isrc -o bin/feature_test.o tests/feature_test.sv");

    print("yosys");
    os.system("yosys -q -p 'read_verilog -Isrc -sv tests/feature_test.sv;'");

    print("done");
    sys.exit(0);
    """

    errors += test_convert_good()
    errors += test_convert_bad()
    errors += test_verilator_parse()
    errors += test_yosys_parse()
    errors += test_icarus_parse()

    if not basic:
        errors += test_compilation()
        errors += test_examples()
        errors += test_misc()
        errors += test_goldens()

        # Lockstep tests are slow because compiler...
        errors += test_lockstep()

        pass

    ############################################################

    print()
    if errors > 0:
        print_r(f"Total failures : {errors}")
        print()
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
    print()
    return errors


################################################################################

def get_pool():
    max_threads = multiprocessing.cpu_count()
    if "--coverage" in sys.argv:
        max_threads = 1
    if "--serial" in sys.argv:
        max_threads = 1
    return multiprocessing.Pool(max_threads)


def metron_default_args():
    #return "-v -e"
    #return "-q"
    return ""


def metron_good():
    result = glob.glob("tests/metron_good/*.h")
    result.sort()
    return result
    #return glob.glob("tests/metron_good/all_func_types.h")


def metron_bad():
    return glob.glob("tests/metron_bad/*.h")


def kcov_prefix():
    if "--coverage" in sys.argv:
        return "kcov --exclude-region=KCOV_OFF:KCOV_ON --include-pattern=Metron --exclude-pattern=submodules --exclude-line=debugbreak coverage"
    else:
        return ""


def print_c(color, *args):
    sys.stdout.write(
        f"\u001b[38;2;{(color >> 16) & 0xFF};{(color >> 8) & 0xFF};{(color >> 0) & 0xFF}m")
    print(*args)
    sys.stdout.write("\u001b[0m")
    sys.stdout.flush()


def print_r(*args):
    print_c(0xFF8080, *args)


def print_g(*args):
    print_c(0x80FF80, *args)


def print_b(*args):
    print_c(0x8080FF, *args)


def prep_cmd(cmd):
    cmd = cmd.strip()
    if "--coverage" in sys.argv:
        cmd = kcov_prefix() + " " + cmd
    args = [arg for arg in cmd.split(" ") if len(arg)]
    cmd_string = ' '.join(args)
    #print(f"  {cmd_string}")
    return args

################################################################################
# Check that the given file is a syntactically valid C++ header.


def check_compile(file):
    cmd = f"  g++ -Isrc --std=gnu++2a -fsyntax-only -c {file}"
    print(cmd)
    return os.system(cmd)

################################################################################
# Check that Metron can translate the source file to SystemVerilog


def check_good(filename):
    errors = 0
    basename = path.basename(filename)
    svname = path.splitext(basename)[0] + ".sv"

    cmd = f"bin/metron {metron_default_args()} -c tests/metron_good/{basename} -o tests/metron_sv/{svname}"

    print(f"  {cmd}");

    cmd_result = subprocess.run(
        prep_cmd(cmd), stdout=subprocess.PIPE, encoding="charmap")

    if cmd_result.returncode:
        print_r(
            f"Test file {filename} - expected pass, got {cmd_result.returncode}")
        print(cmd_result.stdout)
        #result = os.system(f"bin/metron {filename}")
        errors += 1
    return errors

###############################################################################
# Check that the given source does _not_ translate cleanly


def check_bad(filename):
    errors = 0
    basename = path.basename(filename)
    svname = path.splitext(basename)[0] + ".sv"

    lines = open(filename).readlines()
    expected_errors = [line[4:].strip()
                       for line in lines if line.startswith("// X ")]
    if len(expected_errors) == 0:
        print(f"Test {filename} contained no expected errors. Dumping output.")
        # return 1

    cmd = f"bin/metron {metron_default_args()} -c tests/metron_bad/{basename} -o tests/metron_sv/{svname}"
    print(f"  {cmd}")

    cmd_result = subprocess.run(
        prep_cmd(cmd), stdout=subprocess.PIPE, stderr=subprocess.PIPE, encoding="charmap")

    if cmd_result.returncode == 0:
        print(
            f"Test file {filename} - expected fail, got {cmd_result.returncode}")
        errors += 1
        pass
    elif cmd_result.returncode == 34048:
        print(
            f"Test file {filename} - expected fail, but it threw an exception")
        errors += 1
    elif len(expected_errors) == 0:
        print(f"Test {filename} contained no expected errors.")
        pass
    else:
        for err in expected_errors:
            if not err in cmd_result.stdout:
                print(
                    f"Test {filename} did not produce expected error \"{err}\".")
                errors += 1

    if errors:
        print(cmd_result.stdout)
    return errors

################################################################################
# Run Icarus on the translated source file.


def check_icarus(filename):
    errors = 0
    basename = path.basename(filename)
    svname = path.splitext(basename)[0] + ".sv"

    cmd = f"iverilog -g2012 -Wall -Isrc -o bin/{svname}.o tests/metron_sv/{svname}"

    print(f"  {cmd}")

    cmd_result = subprocess.run(
        prep_cmd(cmd), stdout=subprocess.PIPE, stderr=subprocess.PIPE, encoding="charmap")

    if cmd_result.returncode:
        print(f"Icarus syntax check on {filename} failed")
        print(cmd_result.stdout)
        print(cmd_result.stderr)
        return 1
    else:
        return 0

###############################################################################
# Run Verilator on the translated source file.


def check_verilator(filename):
    errors = 0
    basename = path.basename(filename)
    svname = path.splitext(basename)[0] + ".sv"

    cmd = f"verilator -Isrc --lint-only tests/metron_sv/{svname}"
    print(f"  {cmd}")
    result = os.system(cmd)
    if result:
        print(f"Verilator syntax check on {filename} failed")
        return 1
    else:
        return 0

###############################################################################
# Run Yosys on the translated source file.


"""
ninja.rule(name="yosys",
           command="yosys -q -p 'read_verilog ${includes} -sv ${in}; dump; synth_ice40 -json ${out};'")
"""


def check_yosys(filename):
    errors = 0
    basename = path.basename(filename)
    svname = path.splitext(basename)[0] + ".sv"

    cmd = f"yosys -q -p 'read_verilog -Isrc -sv tests/metron_sv/{svname};'"

    print(f"  {cmd}")
    result = os.system(cmd)
    if result:
        print(f"  Yosys syntax check on {filename} failed")
        return 1
    else:
        return 0

###############################################################################
# Check the translated source against the golden, if present.


def check_golden(filename):
    errors = 0
    basename = path.basename(filename)
    svname = path.splitext(basename)[0] + ".sv"
    test_filename = "tests/metron_sv/" + svname
    golden_filename = "tests/metron_golden/" + svname

    try:
        test_src = open(test_filename, "r").read()
        golden_src = open(golden_filename, "r").read()
        if (test_src != golden_src):
            print_r(f"  Mismatch,  {test_filename} != {golden_filename}")
            errors += 1
        else:
            print(f"  {test_filename} == {golden_filename}")
    except:
        print_b(f"  No golden for {golden_filename}")
    return errors

###############################################################################
# Run a command that passes if the output contains "All tests pass"


def run_simple_test(commandline):
    print(f"  {commandline}")
    # The Icarus output isn't actually a binary, kcov can't run it.
    if (commandline == "bin/examples/uart_iv"):
        cmd = [commandline]
    else:
        cmd = prep_cmd(commandline)
    stuff = subprocess.run(cmd, stdout=subprocess.PIPE,
                           encoding="charmap").stdout
    if not "All tests pass" in stuff:
        print_r(stuff)
        return 1
    else:
        return 0

###############################################################################
# Run an arbitrary command as a test


def run_good_command(commandline):
    print(f"  {commandline}");
    cmd = prep_cmd(commandline)
    result = subprocess.run(cmd, stdout=subprocess.PIPE,
                            encoding="charmap").returncode

    if result != 0:
        print(f"Command \"{cmd}\" should have passed, but it failed.")
        return 1
    else:
        return 0


def run_bad_command(commandline):
    print(f"  {commandline}");
    cmd = prep_cmd(commandline)
    result = subprocess.run(cmd, stdout=subprocess.PIPE,
                            encoding="charmap").returncode

    if result == 0:
        print(f"Command \"{cmd}\" should have failed, but it passed.")
        return 1
    else:
        return 0

################################################################################


def test_compilation():
    print()
    print_b("Checking that all headers in tests/metron_good and test/metron_bad compile")
    result = sum(get_pool().map(check_compile, metron_good() + metron_bad()))
    if result != 0:
        print_r(f"Headers in metron_good/metron_bad failed GCC syntax check")
    return result

################################################################################


def test_convert_good():
    print()
    print_b("Checking that all examples in metron_good convert to SV cleanly")
    result = sum(get_pool().map(check_good, metron_good()))
    if result != 0:
        print_r(f"Headers in metron_good failed Metron conversion")
    return result

################################################################################


def test_convert_bad():
    print()
    print_b("Checking that all examples in metron_bad fail conversion")
    errors = sum(get_pool().map(check_bad, metron_bad()))
    if errors != 0:
        print_r(f"Headers in metron_bad passed Metron conversion")
    return errors

################################################################################


def test_verilator_parse():
    print()
    print_b("Checking that all converted files can be parsed by Verilator")
    errors = sum(get_pool().map(check_verilator, metron_good()))
    if errors != 0:
        print_r(f"Headers in metron_good failed Metron conversion")
    return errors

################################################################################

def test_yosys_parse():
    print()
    print_b("Checking that all converted files can be parsed by Yosys")
    errors = sum(get_pool().map(check_yosys, metron_good()))
    if errors != 0:
        print_r(f"Headers in metron_good failed Metron conversion")
    return errors

################################################################################

def test_goldens():
    print()
    print_b("Checking that all converted files match their golden version, if present")
    errors = sum(get_pool().map(check_golden, metron_good()))
    if errors != 0:
        print_r(f"Some headers failed golden check")
    return errors

################################################################################


def test_examples():
    print()
    print_b("Running standalone tests")

    simple_tests = [
        "bin/metron_test",
        "bin/examples/uart",
        "bin/examples/uart_vl",
        "bin/examples/uart_iv",
        # FIXME need to redo these once we have better mem init
        #"bin/examples/rvsimple",
        #"bin/examples/rvsimple_vl",
        "bin/examples/rvsimple_ref",
        #"bin/examples/pinwheel",
        #"bin/examples/pinwheel_vl",
    ]

    errors = sum(get_pool().map(run_simple_test, simple_tests))
    if errors:
        print_r(f"Standalone tests failed")
    return errors

################################################################################


def test_icarus_parse():
    print()
    print_b("Checking that all converted files can be parsed by Icarus")
    errors = sum(get_pool().map(check_icarus, metron_good()))
    if errors != 0:
        print_r(f"Headers in metron_good failed Metron conversion")
    return errors

################################################################################


def test_misc():
    print()
    print_b("Running misc commands")

    good_commands = [
        f"bin/metron {metron_default_args()} -c examples/uart/metron/uart_top.h",
        f"bin/metron {metron_default_args()} -c examples/rvsimple/metron/toplevel.h",
        #f"bin/metron {metron_default_args()} -c examples/pinwheel/metron/pinwheel.h",
        f"bin/metron {metron_default_args()} -c examples/pong/metron/pong.h",
    ]

    bad_commands = [
        f"bin/metron {metron_default_args()} skjdlsfjkhdfsjhdf.h",
        f"bin/metron {metron_default_args()} -c skjdlsfjkhdfsjhdf.h",
    ]

    errors = 0
    errors = errors + sum(get_pool().map(run_good_command, good_commands))
    errors = errors + sum(get_pool().map(run_bad_command, bad_commands))
    return errors

################################################################################


def check_lockstep(filename):
    test_name = filename.rstrip(".h")
    bad_test = "_bad" in filename

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

    includes = f"-I. -Isrc -I{sv_root} -I/usr/local/share/verilator/include"

    metronate_cmd = f"bin/metron -q -c {mt_root}/{test_name}.h -o {sv_root}/{test_name}.sv"
    print(f"  {metronate_cmd}")
    os.system(metronate_cmd)
    os.system(f"verilator {includes} --cc {test_name}.sv -Mdir {vl_root}")
    os.system(f"make --quiet -C {vl_root} -f V{test_name}.mk > /dev/null")
    os.system(
        f"g++ -O3 -std=gnu++2a -DMT_TOP={mt_top} -DVL_TOP={vl_top} -DMT_HEADER={mt_header} -DVL_HEADER={vl_header} {includes} -c {test_src} -o {test_obj}")
    os.system(f"g++ {test_obj} {vl_obj} obj/verilated.o obj/verilated_threads.o -o {test_bin}")

    cmd = f"{test_bin} > /dev/null"
    print(f"  {cmd}");

    errors = os.system(cmd)

    if bad_test:
        return errors == 0
    else:
        return errors


def test_lockstep():
    print()
    print_b("Testing lockstep simulations")

    tests = [
        "counter.h",
        "lfsr.h",
        "funcs_and_tasks.h",
        "lockstep_bad.h",  # expected to fail
        "timeout_bad.h",  # expected to fail
    ]

    os.system(f"mkdir -p gen/tests/metron_lockstep")
    os.system(f"mkdir -p obj/tests/metron_lockstep")
    os.system(f"mkdir -p bin/tests/metron_lockstep")

    errors = 0
    if any(get_pool().map(check_lockstep, tests)):
        errors += 1
    return errors

################################################################################


if __name__ == "__main__":
    sys.exit(main())
