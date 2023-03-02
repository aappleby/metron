# <img src="docs/assets/metron_icon.svg" width="64" height="64"> Metron C++ to Verilog Translator

Metron is a tool for translating a _very_ _limited_ subset of C++ into a _very_ _limited_ subset of SystemVerilog.

Metron accepts plain, unannotated C++ header files as inputs and produces synthesizable SystemVerilog files that are compatible with Icarus, Verilator, Yosys, and most commercial tools.

Metron is _not_ a "High-Level Synthesis" tool. Metron does some basic analysis to determine if your code is semantically compatible with Verilog and then produces a line-by-line translation with only the minimal set of changes needed to make the code compatible with Verilog tooling.

Because it targets hardware implementation, Metron can't handle a lot of C++ language features - no pointers, no virtual functions, no pass-by-reference, limited data structures - but it's still sufficient to build CPUs and peripherals that will run on a FPGA.

# TL;DR:
## [Short essay about hardware vs. software programming](docs/BlueBallMachines.md)
## [Demo](https://aappleby.github.io/Metron/demo/)
## [Tutorial](https://aappleby.github.io/Metron/tutorial/)
## [How Metron Works - Tracing & Symbolic Logic](https://aappleby.github.io/Metron/TemporalLogic.html)

## Metron v0.0.1 Release Notes

First public release!

- Stuff that works
  - Testbench has tests for Verilator, Icarus, and Yosys parsing Metron output.
  - Testbench has pretty good coverage of Metron via kcov.
  - Testbench has test suites for two RISC-V RV32I cores.
  - Testbench has some lock-step tests that check that Metron and Verilator outputs are bit-identical.
  - Tutorial and demo work and have nice responsive live code windows.
- Stuff that doesn't work, but should eventually
  - Struct support. It was kinda working in some tools and not in others and I need to revisit it.
  - Visual Studio support has bit-rotted somewhat.
- Stuff that I'll be doing next
  - Better error reporting. The error _checking_ is good, but the information printed out is pretty useless unless you're in a debugger. I should at least be printing filename + source line for all errors.
  - More complicated examples ported from Verilog to Metron. I have a full-featured UART from OpenCores and the Ibex RISC-V CPU that I've started looking at but they're not functional yet.
  - More chunks of GateBoy/LogicBoy made compatible with Metron.

## FAQ

- I'm already using Verilator to convert my Verilog to C for testing, why would I want to use Metron?
  - Metron can help you prototype new RTL faster than writing it in Verilog.
  - Metron doesn't require a conversion step to integrate with an existing Verilator C++ testbench.
  - Metron modules usually simulate 2x - 5x faster than Verilated modules.
- Wait, what? How fast do Metron models simulate?
  - A trivial LFSR module simulates at 600+ Mhz on my 5900x
  - A 640x480 "Pong" VGA video generator runs at 260+ Mhz with simulated video out via SDL2, or over 10x realtime.
  - A simple RISC-V RV32I core simulates at 360 mhz, though with the caveat that it's a single-cycle core and probably wouldn't synthesize.
  - The UART example in the test bench runs a loopback transmission + checksum at ~400 mhz (the Verilated version is ~130 mhz).
- Have you heard of TLA+?
  - Yes, and I'm aware that I'm also using the phrase "temporal logic", which might confuse some readers. I couldn't think of a better term for the "How Metron Works" page though, alas. Apolgies in advance to Leslie Lamport.
  - It would be interesting to see how Metron programs could use (or be translated into?) TLA+ proofs, but it's out of scope for now.
- Why C++ instead of {language}?
  - When I started the project I wasn't sure if I'd end up needing to do a bunch of brute-force "trace every possible path" stuff, and C++ is my most fluent language so I stuck with it. Now that Metron's working.... yeah, it would probably be simpler if written in Python, and Rust would probably make a better source language for conversion. Might be a future project.

## Building the Metron binary from source:
```
sudo apt install git build-essential ninja-build python3 libicu-dev
git clone --recurse-submodules https://github.com/aappleby/Metron
cd Metron
./build.py
ninja bin/metron
```

## Building everything in the repo:
The full test suite requires quite a bit of stuff. The versions of Verilator and Yosys available via apt are slightly too old and have some bugs, so build them from source - instructions tested on a clean install of Ubuntu 22.04:
```
#sudo apt install verilator    // too old, get verilated_heavy.h error
#sudo apt install yosys        // too old, doesn't like "module uart_hello #(parameter int repeat_msg = 0)"
sudo apt install iverilog      // strongly recommended to build from source
sudo apt install fpga-icestorm // strongly recommended to build from source
sudo apt install nextpnr-ice40 // strongly recommended to build from source
sudo apt install libsdl2-dev
sudo apt install gcc-riscv64-unknown-elf
sudo apt install srecord
```

## Installing Emscripten:
```
cd ~
git clone https://github.com/emscripten-core/emsdk.git
cd emsdk
./emsdk install latest
./emsdk activate latest
source ./emsdk_env.sh
```

## Building Yosys from source:
```
cd ~
sudo apt-get install build-essential clang bison flex libreadline-dev gawk tcl-dev libffi-dev git graphviz xdot pkg-config python3 libboost-system-dev libboost-python-dev libboost-filesystem-dev zlib1g-dev
git clone https://github.com/YosysHQ/yosys
cd yosys
make config-gcc
make -j$(nproc)
sudo make install
```

## Building Verilator from source:
```
cd ~
sudo apt-get install git perl python3 make autoconf g++ flex bison ccache libgoogle-perftools-dev numactl perl-doc libfl2 libfl-dev zlib1g zlib1g-dev help2man
git clone https://github.com/verilator/verilator
cd verilator
autoconf
./configure
make -j$(nproc)
sudo make install
```

## Building Icarus from source:
```
cd ~
sudo apt install build-essential bison flex gperf libreadline-dev autoconf
git clone https://github.com/steveicarus/iverilog
cd iverilog
sh autoconf.sh
./configure
make -j$(nproc)
sudo make install
```

## Building Icestorm from source:
```
cd ~
git clone https://github.com/YosysHQ/icestorm
cd icestorm
sudo apt install build-essential libftdi-dev
make
sudo make install
```

## Building Nextpnr-iCE40 from source (note - requires Icestorm to be installed first):
```
cd ~
git clone https://github.com/YosysHQ/nextpnr
cd nextpnr
sudo apt install python3 python3-dev cmake libboost-dev libboost-filesystem-dev libboost-thread-dev libboost-program-options-dev libboost-iostreams-dev libboost-dev libeigen3-dev
cmake . -DARCH=ice40
make -j$(nproc)
sudo make install
```

## Running the Metron test suite:
```
./build.py
ninja
./run_tests.py
```

## Running test coverage:
```
./build.py
ninja
./run_tests.py --coverage
```
