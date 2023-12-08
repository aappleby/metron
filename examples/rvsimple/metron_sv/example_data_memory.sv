// RISC-V SiMPLE SV -- data memory model
// BSD 3-Clause License
// (c) 2017-2019, Arthur Matos, Marcus Vinicius Lamar, Universidade de Brasília,
//                Marek Materzok, University of Wrocław

`ifndef EXAMPLE_DATA_MEMORY_H
`define EXAMPLE_DATA_MEMORY_H

`include "config.sv"
`include "constants.sv"
`include "metron/metron_tools.sv"

module example_data_memory (
);
 /*public:*/

 /*private:*/
  (* nomem2reg *)
  logic[31:0] mem[2**(rv_config::DATA_BITS - 2)];

 /*public:*/
  always_comb begin : tock
    q = mem[address];
    /*tick();*/
  end

 /*private:*/
  always_comb begin : tick
    if (wren) begin
      logic[31:0] mask;
      // doing this slightly differently from rvsimple so we don't have to do
      // sub-array writes to mem.
      mask = 0;
      if (byteena[0]) mask = mask | 32'h000000FF;
      if (byteena[1]) mask = mask | 32'h0000FF00;
      if (byteena[2]) mask = mask | 32'h00FF0000;
      if (byteena[3]) mask = mask | 32'hFF000000;
      mem[address] = (mem[address] & ~mask) | (data & mask);
    end
  end

 /*public:*/
  parameter /*const char**/ filename = "";
  initial begin
    if (filename) $readmemh(filename, mem);
  end
endmodule

`endif // EXAMPLE_DATA_MEMORY_H
