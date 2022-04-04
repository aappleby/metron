#!/usr/bin/env python3
import os
import glob
from os import path

error = False

os.system("ninja bin/metron")
metron_good = glob.glob("tests/metron_good/*.h")
metron_bad  = glob.glob("tests/metron_bad/*.h")

################################################################################

for filename in metron_good:
  srcdir = "tests/metron_good"
  svdir = "tests/metron_sv"
  basename = path.basename(filename)
  svname = path.splitext(basename)[0] + ".sv"

  print(f"Checking known-good example {filename}");

  cmd = f"g++ -Isrc --std=gnu++2a -fsyntax-only -c {filename}"
  print(f"  {cmd}")
  result = os.system(cmd)
  if result:
    print(f"Test file {filename} failed GCC syntax check");
    error = True

  # Run Metron on the source file
  cmd = f"bin/metron -q -r tests/metron_good -o tests/metron_sv -c {basename}"
  print(f"  {cmd}")
  result = os.system(cmd)
  if result:
    print(f"Test file {filename} - expected pass, got {result}");
    result = os.system(f"bin/metron {filename}")
    error = True

  # Run Verilator on the translated source file.
  cmd = f"verilator -Isrc --lint-only tests/metron_sv/{svname}"
  print(f"  {cmd}")
  result = os.system(cmd)
  if result:
    print(f"Verilator syntax check on {filename} failed");
    error = True

################################################################################

for filename in metron_bad:
  print(f"Checking known-bad example {filename}");

  cmd = f"g++ -Isrc --std=gnu++2a -fsyntax-only -c {filename}"
  print(f"  {cmd}")
  result = os.system(cmd)
  if result:
    print(f"Test file {filename} failed GCC syntax check");
    error = True

  cmd = f"bin/metron -q -r tests/metron_bad -o tets/metron_sv {filename}"
  print(f"  {cmd}")
  result = os.system(cmd)
  if result == 0:
    print(f"Test file {filename} - expected fail, got {result}");
    result = os.system(f"bin/metron {filename}")
    error = True

################################################################################

"""
      ::printf("\u001b[0m");
    } else {
      ::printf("\u001b[38;2;255;0;255m", (color >> 0) & 0xFF,
               (color >> 8) & 0xFF, (color >> 16) & 0xFF);
"""

if error:
  print("\u001b[38;2;255;0;0m")
  print("  █████▒▄▄▄       ██▓ ██▓    ")
  print("▓██   ▒▒████▄    ▓██▒▓██▒    ")
  print("▒████ ░▒██  ▀█▄  ▒██▒▒██░    ")
  print("░▓█▒  ░░██▄▄▄▄██ ░██░▒██░    ")
  print("░▒█░    ▓█   ▓██▒░██░░██████▒")
  print(" ▒ ░    ▒▒   ▓▒█░░▓  ░ ▒░▓  ░")
  print(" ░       ▒   ▒▒ ░ ▒ ░░ ░ ▒  ░")
  print(" ░ ░     ░   ▒    ▒ ░  ░ ░   ")
  print("             ░  ░ ░      ░  ░")
  print("\u001b[0m")
else:
  print("\u001b[38;2;0;255;0m")
  print("██████╗  █████╗ ███████╗███████╗")
  print("██╔══██╗██╔══██╗██╔════╝██╔════╝")
  print("██████╔╝███████║███████╗███████╗")
  print("██╔═══╝ ██╔══██║╚════██║╚════██║")
  print("██║     ██║  ██║███████║███████║")
  print("╚═╝     ╚═╝  ╚═╝╚══════╝╚══════╝")
  print("\u001b[0m")
