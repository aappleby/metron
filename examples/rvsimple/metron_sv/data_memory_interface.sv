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
  input logic[31:0] bus_read_data,
  input logic[31:0] address,
  input logic[2:0] data_format,
  input logic[31:0] write_data,
  output logic[31:0] address2,
  output logic[2:0] data_format2,
  output logic[31:0] bus_write_data,
  output logic[31:0] read_data,
  output logic[3:0] bus_byte_enable
);
 /*public:*/
  /*logic<32> address2;*/
  /*logic<3> data_format2;*/
  /*logic<32> bus_read_data;*/
  /*logic<32> address;*/
  /*logic<3> data_format;*/
  /*logic<32> write_data;*/
  /*logic<32> bus_write_data;*/
  /*logic<32> read_data;*/
  /*logic<4> bus_byte_enable;*/

  always_comb begin /*tock1*/
    address2 = address;
    data_format2 = data_format;
    bus_write_data = write_data << (8 * 2'(address2));

    // calculate byte enable
    // clang-format off
    case (2'(data_format2)) 
      /*case*/ 2'b00: bus_byte_enable = 4'b0001 << 2'(address2); /*break;*/
      /*case*/ 2'b01: bus_byte_enable = 4'b0011 << 2'(address2); /*break;*/
      /*case*/ 2'b10: bus_byte_enable = 4'b1111 << 2'(address2); /*break;*/
      default:   bus_byte_enable = 4'b0000; /*break;*/
    endcase
    // clang-format on
  end

  // correct for unaligned accesses
  always_comb begin /*tock2*/
    logic[31:0] position_fix;
    position_fix = 32'(bus_read_data >> (8 * 2'(address2)));

    // sign-extend if necessary
    // clang-format off
    case (2'(data_format2)) 
      /*case*/ 2'b00: read_data = {{24 {1'(~data_format2[2] & position_fix[7])}}, 8'(position_fix)}; /*break;*/
      /*case*/ 2'b01: read_data = {{16 {1'(~data_format2[2] & position_fix[15])}}, 16'(position_fix)}; /*break;*/
      /*case*/ 2'b10: read_data = 32'(position_fix); /*break;*/
      default:   read_data = 32'bx; /*break;*/
    endcase
    // clang-format on
  end
endmodule;

`endif  // RVSIMPLE_DATA_MEMORY_INTERFACE_H

