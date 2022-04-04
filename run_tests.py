#!/usr/bin/env python3
import os
import glob
import subprocess
from os import path

error = False

os.system("ninja bin/metron")
metron_good = glob.glob("tests/metron_good/*.h")
metron_bad  = glob.glob("tests/metron_bad/*.h")

red_tag   = "\u001b[38;2;255;0;0m"
green_tag = "\u001b[38;2;0;255;0m"
reset_tag = "\u001b[0m"

################################################################################
# Make sure all the examples compile

"""
cmd = f"  g++ -Isrc --std=gnu++2a -fsyntax-only -c {filename}"
print(f"  {cmd}")
result = os.system(cmd)
if result:
  print(f"Test file {filename} failed GCC syntax check");
  error = True
"""

"""
cmd = f"g++ -Isrc --std=gnu++2a -fsyntax-only -c {filename}"
print(f"  {cmd}")
result = os.system(cmd)
if result:
  print(f"Test file {filename} failed GCC syntax check");
  error = True
"""

################################################################################
# Make sure all the good examples pass

for filename in metron_good:
  srcdir = "tests/metron_good"
  svdir = "tests/metron_sv"
  basename = path.basename(filename)
  svname = path.splitext(basename)[0] + ".sv"

  print(f"Checking known-good example {filename}");

  # Run Metron on the source file
  cmd = f"bin/metron -q -r tests/metron_good -o tests/metron_sv -c {basename}"
  print(f"  {cmd}")
  result = os.system(cmd)
  if result:
    print(red_tag + f"Test file {filename} - expected pass, got {result}" + reset_tag);
    result = os.system(f"bin/metron {filename}")
    error = True

  # Run Verilator on the translated source file.
  cmd = f"verilator -Isrc --lint-only tests/metron_sv/{svname}"
  print(f"  {cmd}")
  result = os.system(cmd)
  if result:
    print(f"Verilator syntax check on {filename} failed");
    error = True

  # Check the translated source against the golden, if present.
  try:
    test_src   = open("tests/metron_sv/" + svname, "r").read()
    golden_src = open("tests/metron_ref/" + svname, "r").read()
    if (test_src != golden_src):
      print(red_tag)
      print("Golden mismatch!!!")
      print(reset_tag)
      error = True
  except:
    print(f"  No golden for {filename}")

################################################################################
# Make sure all the bad examples fail

for filename in metron_bad:
  print(f"Checking known-bad example {filename}");

  cmd = f"bin/metron -q -r tests/metron_bad -o tets/metron_sv {filename}"
  print(f"  {cmd}")
  result = os.system(cmd)
  if result == 0:
    print(f"Test file {filename} - expected fail, got {result}");
    result = os.system(f"bin/metron {filename}")
    error = True

################################################################################
# Check uart tests

print("Running test bin/examples/uart")
stuff = subprocess.run(['bin/examples/uart'], stdout=subprocess.PIPE).stdout.decode('utf-8')
if not "All tests pass" in stuff:
  error = True
  print(stuff)

print("Running test bin/examples/uart_vl")
stuff = subprocess.run(['bin/examples/uart_vl'], stdout=subprocess.PIPE).stdout.decode('utf-8')
if not "All tests pass" in stuff:
  error = True
  print(stuff)

print("Running test bin/examples/uart_iv")
stuff = subprocess.run(['bin/examples/uart_iv'], stdout=subprocess.PIPE).stdout.decode('utf-8')
if not "All tests pass" in stuff:
  error = True
  print(stuff)

################################################################################

if error:
  print(red_tag)
  print("  █████▒▄▄▄       ██▓ ██▓    ")
  print("▓██   ▒▒████▄    ▓██▒▓██▒    ")
  print("▒████ ░▒██  ▀█▄  ▒██▒▒██░    ")
  print("░▓█▒  ░░██▄▄▄▄██ ░██░▒██░    ")
  print("░▒█░    ▓█   ▓██▒░██░░██████▒")
  print(" ▒ ░    ▒▒   ▓▒█░░▓  ░ ▒░▓  ░")
  print(" ░       ▒   ▒▒ ░ ▒ ░░ ░ ▒  ░")
  print(" ░ ░     ░   ▒    ▒ ░  ░ ░   ")
  print("             ░  ░ ░      ░  ░")
  print(reset_tag)
else:
  print(green_tag)
  print("██████╗  █████╗ ███████╗███████╗")
  print("██╔══██╗██╔══██╗██╔════╝██╔════╝")
  print("██████╔╝███████║███████╗███████╗")
  print("██╔═══╝ ██╔══██║╚════██║╚════██║")
  print("██║     ██║  ██║███████║███████║")
  print("╚═╝     ╚═╝  ╚═╝╚══════╝╚══════╝")
  print(reset_tag)
