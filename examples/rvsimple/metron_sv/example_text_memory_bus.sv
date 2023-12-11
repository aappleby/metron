// RISC-V SiMPLE SV -- program text memory bus
// BSD 3-Clause License
// (c) 2017-2019, Arthur Matos, Marcus Vinicius Lamar, Universidade de Brasília,
//                Marek Materzok, University of Wrocław

`ifndef EXAMPLE_TEXT_MEMORY_BUS_H
`define EXAMPLE_TEXT_MEMORY_BUS_H

`include "config.sv"
`include "constants.sv"
`include "example_text_memory.sv"
`include "metron/metron_tools.sv"

module example_text_memory_bus (
  // input signals
  input logic[31:0] address,
  // output signals
  output logic[31:0] read_data
);
 /*public:*/

  parameter /*const char**/ filename = "";
  initial begin
  end

 /*private:*/
  example_text_memory #(
    // Constructor Parameters
    .filename(filename)
  ) text_memory(
    // Input signals
    .address(text_memory_address),
    // Output signals
    .q(text_memory_q)
  );
  logic[(rv_config::TEXT_BITS - 2)-1:0] text_memory_address;
  logic[31:0] text_memory_q;

 /*public:*/
  always_comb begin : tock_read_data
    logic[31:0] fetched;
    text_memory_address = address[(rv_config::TEXT_BITS - 2)+1:2];
    /*text_memory.tock_q();*/
    fetched = text_memory_q;
    read_data =
        (address >= rv_config::TEXT_BEGIN) && (rv_config::TEXT_END >= address)
            ? fetched
            : 32'bx;
  end
endmodule

`endif // EXAMPLE_TEXT_MEMORY_BUS_H
