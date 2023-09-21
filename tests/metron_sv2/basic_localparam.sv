`include "metron/tools/metron_tools.sv"

// Static const members become SV localparams

module Module (
  // global clock
  input logic clock,
  // input signals
  input int my_val,
  // output registers
  output logic[6:0] my_reg
);
/*public:*/

  always_comb begin : tock
    /*tick();*/
  end

/*private:*/

  localparam /*static*/ /*const*/ int my_val = 7;

  always_ff @(posedge clock) begin : tick
    my_reg <= my_reg + my_val;
  end

  logic[6:0] my_reg;
endmodule
