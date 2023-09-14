`include "metron/tools/metron_tools.sv"

// Make sure our built-in functions translate.

module Module (
  // output signals
  output int sig
);
/*public:*/


  always_comb begin : tock
    src=100;
    a=$signed(src);
    b=$unsigned(src);
    e=$signed<8>(2'(src));
    c=$clog2(100);
    d=2**(4);
    sig = 1;
  end

endmodule