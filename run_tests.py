#!/usr/bin/env python3
import os
import glob

os.system("ninja bin/metron")
metron_good = glob.glob("tests/metron_good/*.h")
metron_bad  = glob.glob("tests/metron_bad/*.h")

for filename in metron_good:
  print("\n\n");
  result = os.system(f"bin/metron -v {filename}")
  print(f"Result {result}");

for filename in metron_bad:
  print("\n\n");
  result = os.system(f"bin/metron -v {filename}")
  print(f"Result {result}");
