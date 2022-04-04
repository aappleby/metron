#!/usr/bin/env python3
import os
import glob

error = False

os.system("ninja bin/metron")
metron_good = glob.glob("tests/metron_good/*.h")
metron_bad  = glob.glob("tests/metron_bad/*.h")

for filename in metron_good:
  result = os.system(f"bin/metron -q {filename}")
  if result:
    print(f"Test file {filename} - expected pass, got {result}");
    result = os.system(f"bin/metron {filename}")
    error = True
  else:
    print(f"Test file {filename} passed as expected");

for filename in metron_bad:
  result = os.system(f"bin/metron -q {filename}")
  if result == 0:
    print(f"Test file {filename} - expected fail, got {result}");
    result = os.system(f"bin/metron {filename}")
    error = True
  else:
    print(f"Test file {filename} failed as expected");

if error:
  print("Test suite failed!\n")
else:
  print("Test suite passed!\n")
