`include "metron/metron_tools.sv"

// Static const members become SV localparams

module Module (
  // global clock
  input logic clock
);
/*public:*/

  always_ff @(posedge clock) begin : tock
    tick();
  end

/*private:*/

  localparam /*static*/ /*const*/ int my_val = 7;

  task automatic tick();
    my_reg_ <= my_reg_ + my_val;
  endtask

  logic[6:0] my_reg_;
endmodule
