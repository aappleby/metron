// RISC-V SiMPLE SV -- data memory bus
// BSD 3-Clause License
// (c) 2017-2019, Arthur Matos, Marcus Vinicius Lamar, Universidade de Brasília,
//                Marek Materzok, University of Wrocław

`ifndef EXAMPLE_DATA_MEMORY_BUS
`define EXAMPLE_DATA_MEMORY_BUS

`include "config.sv"
`include "constants.sv"
`include "example_data_memory.sv"
`include "metron/metron_tools.sv"

module example_data_memory_bus (
);
 /*public:*/
       // in
     // out
    // in
    // in
    // in
   // in

  parameter /*const char**/ filename = "";
  initial begin
  end

 /*private:*/
  example_data_memory #(
    // Constructor Parameters
    .filename(filename)
  ) data_memory(
  );

 /*public:*/
  always_comb begin : tock
    logic is_data_memory;
    logic[31:0] fetched;
    is_data_memory =
        address >= rv_config::DATA_BEGIN && rv_config::DATA_END >= address;

    data_memory_address = address[(rv_config::DATA_BITS - 2)+1:2];
    data_memory_byteena = byte_enable;
    data_memory_data = write_data;
    data_memory_wren = write_enable & is_data_memory;
    /*data_memory.tock();*/

    fetched = data_memory_q;
    read_data = read_enable && is_data_memory ? fetched : 32'bx;
  end
endmodule

`endif // EXAMPLE_DATA_MEMORY_BUS
