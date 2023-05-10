`include "metron/tools/metron_tools.sv"

module Module (
  // func() ports
  output logic[7:0] func_ret
);
/*public*/

  always_comb begin : func
    parameter /*static*/ /*const*/ thing1 = 1;
    parameter /*const*/ thing2 = 2;
    func_ret = thing1 + thing2;
  end

endmodule
