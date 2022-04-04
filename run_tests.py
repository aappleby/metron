#!/usr/bin/env python3
import os
import glob

os.system("ninja bin/metron")
metron_good = glob.glob("tests/metron_good/*.h")
metron_bad  = glob.glob("tests/metron_bad/*.h")

for filename in metron_good:
  result = os.system(f"bin/metron -q {filename}")
  print(f"Result {result}");

for filename in metron_bad:
  result = os.system(f"bin/metron -q {filename}")
  print(f"Result {result}");
