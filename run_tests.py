#!/usr/bin/env python3
import os
import sys
import glob
import subprocess
from os import path

error = False
metron_good = glob.glob("tests/metron_good/*.h")
metron_bad = glob.glob("tests/metron_bad/*.h")


def print_c(color, *args):
    sys.stdout.write(
        f"\u001b[38;2;{(color >> 16) & 0xFF};{(color >> 8) & 0xFF};{(color >> 0) & 0xFF}m")
    print(*args)
    sys.stdout.write("\u001b[0m")


def print_r(*args):
    print_c(0xFF8080, *args)


def print_g(*args):
    print_c(0x80FF80, *args)


def print_b(*args):
    print_c(0x8080FF, *args)

################################################################################


print()
print_b(" ###    ### ####### ######## ######   ######  ###    ## ")
print_b(" ####  #### ##         ##    ##   ## ##    ## ####   ## ")
print_b(" ## #### ## #####      ##    ######  ##    ## ## ##  ## ")
print_b(" ##  ##  ## ##         ##    ##   ## ##    ## ##  ## ## ")
print_b(" ##      ## #######    ##    ##   ##  ######  ##   #### ")
print()

################################################################################

print_b("Refreshing build")

print()
os.system("ninja bin/metron")

print()

################################################################################

print_b("Checking that all headers in tests/metron_good and test/metron_bad compile")

if 1:
    all_headers = " ".join(metron_good + metron_bad)
    cmd = f"  g++ -Isrc --std=gnu++2a -fsyntax-only -c {all_headers}"
    print(cmd)
    result = os.system(cmd)
    if result:
        print(f"Headers in metron_good/metron_bad failed GCC syntax check")
        error = True

print()

################################################################################

print_b("Checking that all examples in metron_good convert to SV cleanly")

for filename in metron_good:
    srcdir = "tests/metron_good"
    svdir = "tests/metron_sv"
    basename = path.basename(filename)
    svname = path.splitext(basename)[0] + ".sv"

    print(f"Checking known-good example {filename}")

    # Run Metron on the source file
    cmd = f"bin/metron -q -r tests/metron_good -o tests/metron_sv -c {basename}"
    #print(f"  {cmd}")
    result = os.system(cmd)
    if result:
        print_r(f"Test file {filename} - expected pass, got {result}")
        result = os.system(f"bin/metron {filename}")
        error = True

    # Run Verilator on the translated source file.
    cmd = f"verilator -Isrc --lint-only tests/metron_sv/{svname}"
    #print(f"  {cmd}")
    result = os.system(cmd)
    if result:
        print(f"Verilator syntax check on {filename} failed")
        error = True

    # Check the translated source against the golden, if present.
    try:
        test_src = open("tests/metron_sv/" + svname, "r").read()
        golden_src = open("tests/metron_ref/" + svname, "r").read()
        if (test_src != golden_src):
            print_r("Golden mismatch!!!")
            error = True
    except:
        print_b(f"  No golden for {filename}")

print()

################################################################################

print_b("Checking that all examples in metron_bad fail conversion")

for filename in metron_bad:
    srcdir = "tests/metron_bad"
    svdir = "tests/metron_sv"
    basename = path.basename(filename)
    svname = path.splitext(basename)[0] + ".sv"

    print(f"Checking known-bad example {filename}")

    cmd = f"bin/metron -q -r tests/metron_bad -o tests/metron_sv -c {basename}"
    #print(f"  {cmd}")
    result = os.system(cmd)
    if result == 0:
        print(f"Test file {filename} - expected fail, got {result}")
        result = os.system(f"bin/metron {filename}")
        error = True

print()

################################################################################
# Check standalone tests

print_b("Running standalone tests")


def run_simple_test(commandline):
    print(f"Running test {commandline}")
    stuff = subprocess.run(commandline.split(
        " "), stdout=subprocess.PIPE).stdout.decode('utf-8')
    error = not "All tests pass" in stuff
    if error:
        print_r(stuff)
    return error


error |= run_simple_test("bin/examples/uart")
error |= run_simple_test("bin/examples/uart_vl")
error |= run_simple_test("bin/examples/uart_iv")

error |= run_simple_test("bin/examples/rvsimple")
error |= run_simple_test("bin/examples/rvsimple_vl")
error |= run_simple_test("bin/examples/rvsimple_ref")

error |= run_simple_test("bin/examples/rvtiny")
error |= run_simple_test("bin/examples/rvtiny_vl")

error |= run_simple_test("bin/examples/rvtiny_sync")
error |= run_simple_test("bin/examples/rvtiny_sync_vl")

print()

################################################################################

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
