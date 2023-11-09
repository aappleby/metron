`include "metron/metron_tools.sv"

typedef union packed {
  uint32_t a;
  uint32_t b;
} my_union;

module Module (
  // output signals
  output my_union blah,
  // tock() ports
  input uint32_t tock_x,
  input uint32_t tock_y
);
/*public:*/

  always_comb begin : tock
    blah.a = tock_x;
    blah.b = tock_y;
  end

endmodule
