# Metron, a tool for writing synthesizable Verilog in C++

## [Demo!](https://aappleby.github.io/Metron/demo/)
## [Tutorial](https://aappleby.github.io/Metron/tutorial/)
## [Temporal Logic](https://aappleby.github.io/Metron/TemporalLogic.html)

Requirements for Metron itself (Linux):
- gcc
- python3
- ninja

Requirements for Metron itself (Windows):
- Visual Studio 2022 Community

Requirements for Metron's test suite
- verilator
- yosys
- nextpnr
- riscv64-unknown-elf-* toolchain
- (I highly recommend building these from source)

```
sudo apt install git build-essential ninja-build python3 libicu-dev
git clone --recurse-submodules https://github.com/aappleby/Metron
cd Metron
./build.py
ninja
```


```
sudo apt install verilator // too old, get verilated_heavy.h error
sudo apt install iverilog
sudo apt install nextpnr-ice40
sudo apt install yosys
sudo apt install fpga-icestorm


sudo apt install libsdl2-dev


git clone https://github.com/verilator/verilator





# Prerequisites:
sudo apt-get install git perl python3 make autoconf g++ flex bison ccache libgoogle-perftools-dev numactl perl-doc libfl2 libfl-dev zlib1g zlib1g-dev

git clone https://github.com/verilator/verilator   # Only first time

# Every time you need to build:
unsetenv VERILATOR_ROOT  # For csh; ignore error if on bash
unset VERILATOR_ROOT  # For bash
cd verilator
git pull         # Make sure git repository is up-to-date
git tag          # See what versions exist
#git checkout master      # Use development branch (e.g. recent bug fixes)
#git checkout stable      # Use most recent stable release
#git checkout v{version}  # Switch to specified release version

autoconf         # Create ./configure script
./configure      # Configure and create Makefile
make -j `nproc`  # Build Verilator itself (if error, try just 'make')
sudo make install

```

git submodule init
git submodule update


Building Metron:
- ./build.py
- ninja

Testing Metron:
- ./run_tests.py


Example counter:
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

Example counter converted by Metron:
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
