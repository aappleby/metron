`include "metron/tools/metron_tools.sv"

// Namespaces turn into packages.
// "using" doesn't work in methods right now :/

package MyPackage;
  parameter int foo = 3;
endpackage

module Module (
  // global clock
  input logic clock,
  // output signals
  output int my_sig,
  // output registers
  output int my_reg,
  // tock() ports
  output int tock_ret
);
/*public*/


  always_comb begin : tock
    my_sig = MyPackage::foo + 1;
    tock_ret = my_sig;
  end

  always_ff @(posedge clock) begin : tick
    my_reg <= my_reg + MyPackage::foo;
  end
endmodule
