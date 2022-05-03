`include "metron_tools.sv"

// Structs exist in both C++ and Verilog

typedef struct packed
  logic[7:0] a;
  int b;
  logic[1:0] c;
endmodule MyStruct1;

typedef struct packed
  logic[7:0] a;
  int b;
  logic[1:0] c;
endmodule MyStruct2;;

module Module
(
  input logic clock,
  output logic[7:0] tock_ret
);
/*public:*/
  function logic[7:0] tock();
    MyStruct1 my_struct1;
    MyStruct2 my_struct2;

    MyStruct1 my_struct1;
    my_struct1.a = 8'd1234;
    my_struct1.b = 12;
    my_struct1.c = 2'b01;

    MyStruct2 my_struct2;
    my_struct2.a = 8'd1234;
    my_struct2.b = 12;
    my_struct2.c = 2'b01;

    tock = 17;
  endfuction
  always_comb tock_ret = tock();
endmodule;

