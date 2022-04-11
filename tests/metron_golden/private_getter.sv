`include "metron_tools.sv"

// Private getter methods are OK

module Module
(
  input logic clock,
  output logic[7:0] tock
);
/*public:*/

  always_comb begin /*tock*/
    tock = my_getter();
  end

/*private:*/

  function logic[7:0] my_getter();
    my_getter = 12;
  endfunction

endmodule

