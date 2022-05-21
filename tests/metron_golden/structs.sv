`include "metron_tools.sv"

// Structs exist in both C++ and Verilog

typedef struct packed {
  logic[7:0] a;
} MyStruct1;

module Module
(
  // output signals
  output MyStruct1 my_struct1,
  // tock() ports
  output logic[7:0] tock_ret
);
/*public:*/


  always_comb begin : tock
    my_struct1.a = 1;
    tock_ret = 17;
  end
endmodule
