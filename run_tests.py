#!/usr/bin/env python3
import os
import sys
import glob
import subprocess
import multiprocessing
import time
from os import path

coverage = False

for i, arg in enumerate(sys.argv):
    if (arg == "--coverage"):
        coverage = True

kcov_prefix = "kcov --exclude-region=KCOV_OFF:KCOV_ON --include-pattern=Metron --exclude-pattern=submodules coverage " if coverage else ""


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

################################################################################
# Check that the given file is a syntactically valid C++ header.

def check_compile(file):
    cmd = f"  g++ -Isrc --std=gnu++2a -fsyntax-only -c {file}"
    print(cmd)
    return os.system(cmd)

################################################################################
# Check that Metron can translate the source file to SystemVerilog

def check_good(filename):
    error = 0
    basename = path.basename(filename)
    svname = path.splitext(basename)[0] + ".sv"

    cmd = kcov_prefix + f"bin/metron -q -r tests/metron_good -o tests/metron_sv -c {basename}"
    print(f"  {cmd}")
    result = os.system(cmd)
    if result:
        print_r(f"Test file {filename} - expected pass, got {result}")
        result = os.system(f"bin/metron {filename}")
        error = 1
    return error

################################################################################
# Run Icarus on the translated source file.

def check_icarus(filename):
    error = 0
    basename = path.basename(filename)
    svname = path.splitext(basename)[0] + ".sv"

    cmd = f"iverilog -g2012 -Wall -Isrc -o bin/{svname}.o tests/metron_sv/{svname}"
    result = os.system(cmd)
    if result:
        print(f"Icarus syntax check on {filename} failed")
        error = 1
    return error

###############################################################################
# Run Verilator on the translated source file.

def check_verilator(filename):
    error = 0
    basename = path.basename(filename)
    svname = path.splitext(basename)[0] + ".sv"

    cmd = f"verilator -Isrc --lint-only tests/metron_sv/{svname}"
    print(f"  {cmd}")
    result = os.system(cmd)
    if result:
        print(f"Verilator syntax check on {filename} failed")
        error = 1
    return error

###############################################################################
# Check the translated source against the golden, if present.

def check_golden(filename):
    error = 0
    basename = path.basename(filename)
    svname = path.splitext(basename)[0] + ".sv"
    test_filename = "tests/metron_sv/" + svname
    golden_filename = "tests/metron_ref/" + svname

    try:
        test_src = open(test_filename, "r").read()
        golden_src = open(golden_filename, "r").read()
        if (test_src != golden_src):
          print_r(f"  Mismatch,  {test_filename} != {golden_filename}")
          error = 1
        else:
          print(f"  {test_filename} == {golden_filename}")
    except:
        print_b(f"  No golden for {golden_filename}")
    return error

###############################################################################
# Check that the given source does _not_ translate cleanly

def check_bad(filename):
    error = 0
    basename = path.basename(filename)
    svname = path.splitext(basename)[0] + ".sv"

    cmd = kcov_prefix + f"bin/metron -q -r tests/metron_bad -o tests/metron_sv -c {basename}"
    print(f"  {cmd}")
    result = os.system(cmd)
    if result == 0:
        print(f"Test file {filename} - expected fail, got {result}")
        result = os.system(f"bin/metron {filename}")
        error = 1
    elif result == 34048:
      print(f"Test file {filename} - expected fail, but it threw an exception")
      error = 1
    return error

###############################################################################
# Run a command that passes if the output contains "All tests pass"

def run_simple_test(commandline):
    cmd = kcov_prefix + commandline
    error = 0
    print(f"  {cmd}")
    stuff = subprocess.run(cmd.split(
        " "), stdout=subprocess.PIPE).stdout.decode('utf-8')
    if not "All tests pass" in stuff:
        print_r(stuff)
        error = 1
    return error

###############################################################################
# Run an arbitrary command as a test

def run_good_command(commandline):
    cmd = kcov_prefix + commandline
    error = 0
    print(f"  {cmd}")

    result = os.system(cmd)
    if result != 0:
        print(f"Command \"{cmd}\" should have passed, but it failed.")
        error = 1
    return error

def run_bad_command(commandline):
    cmd = kcov_prefix + commandline
    error = 0
    print(f"  {cmd}")

    result = os.system(cmd)
    if result == 0:
        print(f"Command \"{cmd}\" should have failed, but it passed.")
        error = 1
    return error

################################################################################


if __name__ == "__main__":
    print()
    print_b(" ###    ### ####### ######## ######   ######  ###    ## ")
    print_b(" ####  #### ##         ##    ##   ## ##    ## ####   ## ")
    print_b(" ## #### ## #####      ##    ######  ##    ## ## ##  ## ")
    print_b(" ##  ##  ## ##         ##    ##   ## ##    ## ##  ## ## ")
    print_b(" ##      ## #######    ##    ##   ##  ######  ##   #### ")

    ############################################################

    print()
    print_b("Refreshing build")
    if os.system("ninja"):
        print("Build failed!")
        sys.exit(-1)

    ############################################################

    metron_good = glob.glob("tests/metron_good/*.h")
    metron_bad = glob.glob("tests/metron_bad/*.h")

    pool = multiprocessing.Pool(1) if coverage else multiprocessing.Pool()
    #pool = multiprocessing.Pool(1)
    error = False

    ############################################################

    print()
    print_b("Checking that all headers in tests/metron_good and test/metron_bad compile")
    if any(pool.map(check_compile, metron_good + metron_bad)):
        print_r(f"Headers in metron_good/metron_bad failed GCC syntax check")
        error = True

    print()
    print_b("Checking that all examples in metron_good convert to SV cleanly")
    if any(pool.map(check_good, metron_good)):
        print_r(f"Headers in metron_good failed Metron conversion")
        error = True

    print()
    print_b("Checking that all converted files can be parsed by Verilator")
    if any(pool.map(check_verilator, metron_good)):
        print_r(f"Headers in metron_good failed Metron conversion")
        error = True

    # Icarus generates a bunch of possibly-spurious warnings and can't handle
    # utf-8 with a BOM

    """
    print()
    print_b("Checking that all converted files can be parsed by Icarus")
    if any(pool.map(check_icarus, metron_good)):
        print_r(f"Headers in metron_good failed Metron conversion")
        error = True
    """

    print()
    print_b("Checking that all converted files match their golden version, if present")
    if any(pool.map(check_golden, metron_good)):
        print_r(f"Some headers failed golden check")
        error = True

    print()
    print_b("Checking that all examples in metron_bad fail conversion")
    if any(pool.map(check_bad, metron_bad)):
        print_r(f"Headers in metron_bad passed Metron conversion")
        error = True

    ############################################################

    print()
    print_b("Running standalone tests")

    simple_tests = [
        "bin/metron_test",
        "bin/examples/uart",
        "bin/examples/uart_vl",
        "bin/examples/uart_iv",
        "bin/examples/rvsimple",
        "bin/examples/rvsimple_vl",
        "bin/examples/rvsimple_ref",
        "bin/examples/rvtiny",
        "bin/examples/rvtiny_vl",
        "bin/examples/rvtiny_sync",
        "bin/examples/rvtiny_sync_vl",
    ]

    if any(pool.map(run_simple_test, simple_tests)):
        print_r(f"Standalone tests failed")
        error = True

    ############################################################

    print()
    print_b("Running misc commands")

    good_commands = [
        "bin/metron -e examples/rvtiny/metron/toplevel.h > /dev/null",
        "bin/metron -v examples/rvtiny/metron/toplevel.h > /dev/null",
        "bin/metron --dump examples/rvtiny/metron/toplevel.h > /dev/null",
    ]

    bad_commands = [
        "bin/metron skjdlsfjkhdfsjhdf.h > /dev/null",
        "bin/metron -c skjdlsfjkhdfsjhdf.h > /dev/null",
        "bin/metron -o sdkjfshkdjfshyry skjdlsfjkhdfsjhdf.h > /dev/null",
    ]

    if any(pool.map(run_good_command, good_commands)):
        error = True

    if any(pool.map(run_bad_command, bad_commands)):
        error = True

    ############################################################

    print()
    if error:
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
