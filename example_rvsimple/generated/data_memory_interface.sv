// RISC-V SiMPLE SV -- data memory interface
// BSD 3-Clause License
// (c) 2017-2019, Arthur Matos, Marcus Vinicius Lamar, Universidade de Brasília,
//                Marek Materzok, University of Wrocław

`ifndef RVSIMPLE_DATA_MEMORY_INTERFACE_H
`define RVSIMPLE_DATA_MEMORY_INTERFACE_H

`include "config.sv"
`include "constants.sv"
`include "metron_tools.sv"

module data_memory_interface
(
  input logic clock,
  input logic[31:0] address,
  input logic[31:0] write_data,
  input logic[2:0] data_format,
  input logic[31:0] bus_read_data,
  output logic[31:0] bus_write_data,
  output logic[3:0] bus_byte_enable,
  output logic[31:0] read_data
);
 /*public:*/

  always_comb begin
    bus_write_data = write_data << (8 * 2'(address));
  end

  always_comb begin
    // calculate byte enable
    case (2'(data_format)) 
      /*case*/ 2'b00: bus_byte_enable = 4'b0001 << 2'(address);
      /*case*/ 2'b01: bus_byte_enable = 4'b0011 << 2'(address);
      /*case*/ 2'b10: bus_byte_enable = 4'b1111 << 2'(address);
      default:   bus_byte_enable = 4'b0000;
    endcase
  end

  always_comb begin
    logic[31:0] position_fix;
    // correct for unaligned accesses
    position_fix = 32'(bus_read_data >> (8 * 2'(address)));

    // sign-extend if necessary
    case (2'(data_format)) 
      /*case*/ 2'b00: read_data = {{24 {1'(~data_format[2] & position_fix[7])}}, 8'(position_fix)};
      /*case*/ 2'b01: read_data = {{16 {1'(~data_format[2] & position_fix[15])}}, 16'(position_fix)};
      /*case*/ 2'b10: read_data = 32'(position_fix);
      default:   read_data = 32'bx;
    endcase
  end
endmodule;

`endif  // RVSIMPLE_DATA_MEMORY_INTERFACE_H

