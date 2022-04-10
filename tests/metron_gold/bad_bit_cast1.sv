`include "metron_tools.sv"

// We should catch mismatched bit casts in initializers. Other implicit bit
// casts are allowed by SV and I'm not sure if we should catch them.

module Module
(
  input logic clock
);
/*public:*/

  always_comb begin /*tock1*/
    logic[63:0] src;
    logic[2:0] dst63;
    src = 64'h1234567812345678;
    dst63 = 7'(src);
  end

  /*
  static const int some_size1 = 23;
  static const int some_size2 = 12;
  static const int some_size3 = 7;

  void tock2() {
    logic<some_size1> a = 10;
    logic<some_size2> b = bx<some_size3>(a);
  }
  */
endmodule

