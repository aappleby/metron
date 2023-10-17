`include "metron/metron_tools.sv"

module Module (
  // foo() ports
  output logic foo_ret,
  // bar() ports
  output int bar_ret
);
  /*public:*/

  always_comb begin : foo
    foo_ret = 0;
  end

  // metron_noconvert
  /*int bar() {
    return 0xDEADBEEF;
  }*/

endmodule
