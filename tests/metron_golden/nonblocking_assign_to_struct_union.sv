`include "metron/metron_tools.sv"

// Structs exist in both C++ and Verilog

typedef struct packed {
  logic[7:0] a;
  logic[7:0] b;
} MyStruct1;

typedef union packed {
  logic[7:0] a;
  logic[7:0] b;
} MyUnion1;

module Module (
  // global clock
  input logic clock
);
/*public:*/

  always_ff @(posedge clock) begin : tick
    my_struct_.a <= my_struct_.b + 1;
    my_union_.a <= my_union_.b + 1;
  end

/*private:*/

  MyStruct1 my_struct_;
  MyUnion1 my_union_;

endmodule
