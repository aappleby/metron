// RISC-V SiMPLE SV -- Toplevel
// BSD 3-Clause License
// (c) 2017-2019, Arthur Matos, Marcus Vinicius Lamar, Universidade de Brasília,
//                Marek Materzok, University of Wrocław

`ifndef TOPLEVEL_H
`define TOPLEVEL_H

`include "config.sv"
`include "constants.sv"
`include "example_data_memory_bus.sv"
`include "example_text_memory_bus.sv"
`include "metron/metron_tools.sv"
`include "riscv_core.sv"

module toplevel (
  // tock() ports
  input int tock_bogus_param
);
 /*public:*/


  parameter /*const char**/ text_filename = "";
  parameter /*const char**/ data_filename = "";
  initial begin
  end

  //----------------------------------------

  always_comb begin : tock
    core_reset = reset;
    /*core.tock_pc();*/

    text_memory_bus_address = core_pc;
    /*text_memory_bus.tock_read_data();*/

    core_inst = text_memory_bus_read_data;
    /*core.tock_execute();*/

    data_memory_bus_address = core_bus_address;
    data_memory_bus_read_enable = core_bus_read_enable;
    data_memory_bus_write_enable = core_bus_write_enable;
    data_memory_bus_byte_enable = core_bus_byte_enable;
    data_memory_bus_write_data = core_bus_write_data;
    /*data_memory_bus.tock();*/

    core_bus_read_data = data_memory_bus_read_data;
    /*core.tock_writeback();*/

    //----------

    bus_read_data = data_memory_bus_read_data;
    bus_address = core_bus_address;
    bus_write_data = core_bus_write_data;
    bus_byte_enable = core_bus_byte_enable;
    bus_read_enable = core_bus_read_enable;
    bus_write_enable = core_bus_write_enable;
    inst = text_memory_bus_read_data;
    pc = core_pc;
  end

  //----------------------------------------

 /*private:*/
  riscv_core core(
  );

  example_text_memory_bus #(
    // Constructor Parameters
    .filename(text_filename)
  ) text_memory_bus(
  );
  example_data_memory_bus #(
    // Constructor Parameters
    .filename(data_filename)
  ) data_memory_bus(
  );
endmodule

`endif // TOPLEVEL_H
