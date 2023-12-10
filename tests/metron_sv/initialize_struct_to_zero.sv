`include "metron/metron_tools.sv"

// Zero-initializing structs should work for convenience.

typedef struct packed {
  logic[7:0] field;
} MyStruct1;

module Module (
  // output signals
  output logic[7:0] field
);
/*public:*/

  always_comb begin : tock
    // FIXME fix this later glarghbh
    //my_struct1 = {0};
    my_struct1.field = 0;
  end
endmodule
