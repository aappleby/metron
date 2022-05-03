`include"metron_tools.sv"

// Namespaces turn into packages.

package MyPackage;
  parameter int foo = 3;
endpackage

module Module
(
  input logic clock,
  output logic[7:0]tock1_ret,
  output logic[7:0]tock2_ret
);
/*public:*/

  function logic[7:0] tock1();
    tock1 = MyPackage::foo;
  endfuction
  always_comb tock1_ret = tock1();

  function logic[7:0] tock2();
    import MyPackage::*;
    tock2 = foo;
  endfuction
  always_comb tock2_ret = tock2();
endmodule;

