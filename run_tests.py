#!/usr/bin/env python3
import os
import sys
import glob
import subprocess
import multiprocessing
import time
from os import path


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


def check_compile(file):
    cmd = f"  g++ -Isrc --std=gnu++2a -fsyntax-only -c {file}"
    print(cmd)
    return os.system(cmd)

################################################################################


def check_good(filename):
    error = 0
    srcdir = "tests/metron_good"
    svdir = "tests/metron_sv"
    basename = path.basename(filename)
    svname = path.splitext(basename)[0] + ".sv"

    print(f"  Checking known-good example {filename}")

    # Run Metron on the source file
    cmd = f"bin/metron -q -r tests/metron_good -o tests/metron_sv -c {basename}"
    result = os.system(cmd)
    if result:
        print_r(f"Test file {filename} - expected pass, got {result}")
        result = os.system(f"bin/metron {filename}")
        error = 1

    # Run Verilator on the translated source file.
    cmd = f"verilator -Isrc --lint-only tests/metron_sv/{svname}"
    result = os.system(cmd)
    if result:
        print(f"Verilator syntax check on {filename} failed")
        error = 1

    # Check the translated source against the golden, if present.
    try:
        test_src = open("tests/metron_sv/" + svname, "r").read()
        golden_src = open("tests/metron_ref/" + svname, "r").read()
        if (test_src != golden_src):
            print_r(f"Golden mismatch for {svname}")
            error = 1
    except:
        print_b(f"  No golden for {filename}")
    return error

############################################################


def check_bad(filename):
    error = 0
    srcdir = "tests/metron_bad"
    svdir = "tests/metron_sv"
    basename = path.basename(filename)
    svname = path.splitext(basename)[0] + ".sv"

    print(f"  Checking known-bad example {filename}")

    cmd = f"bin/metron -q -r tests/metron_bad -o tests/metron_sv -c {basename}"
    #print(f"  {cmd}")
    result = os.system(cmd)
    if result == 0:
        print(f"Test file {filename} - expected fail, got {result}")
        result = os.system(f"bin/metron {filename}")
        error = 1
    return error

############################################################


def run_simple_test(commandline):
    error = 0
    print(f"  Running test {commandline}")
    stuff = subprocess.run(commandline.split(
        " "), stdout=subprocess.PIPE).stdout.decode('utf-8')
    if not "All tests pass" in stuff:
        print_r(stuff)
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

    simple_tests = [
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

    pool = multiprocessing.Pool()
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
    print_b("Checking that all examples in metron_bad fail conversion")
    if any(pool.map(check_bad, metron_bad)):
        print_r(f"Headers in metron_bad passed Metron conversion")
        error = True

    print()
    print_b("Running standalone tests")
    if any(pool.map(run_simple_test, simple_tests)):
        print_r(f"Standalone tests failed")
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
