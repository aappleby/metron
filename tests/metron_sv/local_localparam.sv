`include "metron_tools.sv"

module Module
(
  output logic[7:0] func_ret
);
/*public:*/

  function logic[7:0] func();
    parameter thing1 = 1;
    parameter thing2 = 2;
    func = thing1 + thing2;
  endfunction
  always_comb func_ret = func();

endmodule
