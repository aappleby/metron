`include "metron/metron_tools.sv"

// Namespaces turn into packages.
// "using" doesn't work in methods right now :/

package MyPackage;
  parameter /*static*/ /*const*/ int foo = 3;
endpackage

module Module
(
  // global clock
  input logic clock,
  // output signals
  output int my_sig,
  output int tock_ret,
  // output registers
  output int my_reg_
);
/*public:*/


  always_comb begin : tock
    my_sig = MyPackage::foo + 1;
    tock_ret = my_sig;
  end

  always_ff @(posedge clock) begin : tick
    my_reg_ <= my_reg_ + MyPackage::foo;
  end
endmodule
