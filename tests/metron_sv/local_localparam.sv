`include "metron/metron_tools.sv"

module Module (
  // func() ports
  output logic[7:0] func_ret
);
/*public:*/

  always_comb begin : func
    /*const*/ int thing2;
    parameter /*static*/ /*const*/ int thing1 = 1;
    thing2 = 2;
    func_ret = thing1 + thing2;
  end

endmodule
