# <img src="docs/assets/metron_icon.svg" width="64" height="64"> Metron C++ to Verilog Translator

Metron is a tool for translating a _very_limited_ subset of C++ into a _very_limited_ subset of SystemVerilog.

Metron accepts plain, unannotated C++ header files as inputs and produces SystemVerilog files that are compatible with Icarus, Verilator, Yosys, and most commercial tools.

Metron does some basic code analysis to determine if your C++ code is semantically compatible with Verilog.

Metron is _not_ a "High-Level Synthesis" tool, its translations are mostly line-for-line with only the minimal set of changes needed to make the code look like Verilog.

Metron can't handle most C++ language features and libraries - no pointers, no virtual functions, no pass-by-reference, limited data structures - but it's still sufficient to build CPUs and peripherals that will run on a FPGA.

## FAQ

## [TL;DR: Demo](https://aappleby.github.io/Metron/demo/)
## [Tutorial](https://aappleby.github.io/Metron/tutorial/)
## [Temporal Logic](https://aappleby.github.io/Metron/TemporalLogic.html)

## FAQ

- I'm already using Verilator to convert my Verilog to C for testing, why would I want to use Metron?
  - Metron can help you prototype new RTL faster than writing it in Verilog.
  - Metron doesn't require a conversion step to integrate with an existing Verilator C++ testbench.
  - Metron modules usually simulate 2x - 5x faster than Verilated modules.
- Wait, what? How fast do Metron models simulate?
  - A trivial LFSR module simulates at 600+ Mhz on my 5900x
  - A 640x480 "Pong" VGA video generator runs at 260+ Mhz, over 10x realtime.
  - A simple RISC-V RV32I core simulates at 360 mhz, though with the caveat that it's a single-cycle core and probably wouldn't synthesize.

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
#sudo apt install verilator // too old, get verilated_heavy.h error
#sudo apt install yosys // too old, doesn't like "module uart_hello #(parameter int repeat_msg = 0)"
sudo apt install iverilog
sudo apt install nextpnr-ice40
sudo apt install fpga-icestorm
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
sudo apt-get install build-essential clang bison flex libreadline-dev gawk \
  tcl-dev libffi-dev git graphviz xdot pkg-config python3 libboost-system-dev \
  libboost-python-dev libboost-filesystem-dev zlib1g-dev
git clone https://github.com/YosysHQ/yosys
cd yosys
make config-gcc
make -j$(nproc)
sudo make install
```

## Building Verilator from source:
```
cd ~
sudo apt-get install git perl python3 make autoconf g++ flex bison ccache \
    libgoogle-perftools-dev numactl perl-doc libfl2 libfl-dev zlib1g zlib1g-dev
git clone https://github.com/verilator/verilator
cd verilator
autoconf
./configure
make -j$(nproc)
sudo make install
```

## Running the Metron test suite:
```
./build.py
ninja
./run_tests.py
```


## Example counter:
```
// A very basic counter in plain C++, converted to Verilog using Metron.

class Counter {
public:
  int count;
  void update() {
    count++;
  }
};
```

## Example counter converted by Metron:
```
// A very basic counter in plain C++, converted to Verilog using Metron.

module Counter (
  // global clock
  input logic clock,
  // output registers
  output int count
);
/*public:*/
  always_ff @(posedge clock) begin : update
    count <= count + 1;
  end
endmodule
```
