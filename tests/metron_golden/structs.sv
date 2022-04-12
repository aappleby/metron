`include "metron_tools.sv"

// Structs exist in both C++ and Verilog

typedef struct packed {
  logic[7:0] a;
  int b;
  logic[1:0] c;
} MyStruct;

module Module
(
  input logic clock,
  output logic[7:0] tock
);
/*public:*/
  always_comb begin /*tock*/
    MyStruct my_struct;

    my_struct.a = 8'd1234;
    my_struct.b = 12;
    my_struct.c = 2'b01;
    tock = 17;
  end
endmodule

