#!/usr/bin/env python3
import os
import glob

os.system("ninja bin/metron")
metron_good = glob.glob("tests/metron_good/*.h")
metron_bad  = glob.glob("tests/metron_bad/*.h")

for filename in metron_good:
  os.system(f"bin/metron -v {filename}")

for filename in metron_bad:
  os.system(f"bin/metron -v {filename}")
