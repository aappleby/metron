`include "metron_tools.sv"

// Zero-initializing structs should work for convenience.

typedef struct packed {
  logic[7:0] a;
} MyStruct1;

module Module (
  // tock() ports
  output logic[7:0] tock_ret
);
/*public:*/

  always_comb begin : tock
    //MyStruct1 my_struct1 = {0};
    // FIXME finish this test
    //MyStruct1 my_struct1;
    //my_struct1.a = 1;
    tock_ret = 17;
  end
endmodule
