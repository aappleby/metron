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
