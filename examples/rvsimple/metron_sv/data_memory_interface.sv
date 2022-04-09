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
   output logic[3:0] bus_byte_enable,
   output logic[31:0] read_data
);
 /*public:*/

   /*logic<32> address2;*/
   /*logic<3> data_format2;*/
   /*logic<32> bus_read_data;*/

   always_comb begin /*tock_inputs*/
     address2 = address;
     data_format2 = data_format;
   end

  always_comb begin
    bus_write_data = write_data << (8 * 2'(address2));
  end

  always_comb begin
    logic[3:0] result;
    // calculate byte enable
    /*logic<4> result;*/
    case (2'(data_format2)) 
      /*case*/ 2'b00: result = 4'b0001 << 2'(address2); /*break;*/
      /*case*/ 2'b01: result = 4'b0011 << 2'(address2); /*break;*/
      /*case*/ 2'b10: result = 4'b1111 << 2'(address2); /*break;*/
      default:   result = 4'b0000; /*break;*/
    endcase
    bus_byte_enable = result;
  end

  always_comb begin
    logic[31:0] position_fix;
    logic[31:0] result;
    // correct for unaligned accesses
    position_fix = 32'(bus_read_data >> (8 * 2'(address2)));

    // sign-extend if necessary
    /*logic<32> result;*/
    case (2'(data_format2)) 
      /*case*/ 2'b00: result = {{24 {1'(~data_format2[2] & position_fix[7])}}, 8'(position_fix)}; /*break;*/
      /*case*/ 2'b01: result = {{16 {1'(~data_format2[2] & position_fix[15])}}, 16'(position_fix)}; /*break;*/
      /*case*/ 2'b10: result = 32'(position_fix); /*break;*/
      default:   result = 32'bx; /*break;*/
    endcase
    read_data = result;
  end
endmodule;

`endif  // RVSIMPLE_DATA_MEMORY_INTERFACE_H

