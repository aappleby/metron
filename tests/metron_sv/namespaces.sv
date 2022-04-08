`include "metron_tools.sv"

// Namespaces turn into packages.

package MyPackage;
  parameter int foo = 3;
endpackage;

module Module
(
  input logic clock,
  output logic[7:0] tock1,
  output logic[7:0] tock2
);
/*public:*/

  always_comb begin /*tock1*/
    tock1 = foo;
  end

  always_comb begin /*tock2*/
    import MyPackage::*;
    tock2 = foo;
  end
endmodule

