`include "metron/metron_tools.sv"

module Module (
  // global clock
  input logic clock,
  // output signals
  output int sig,
  // output registers
  output int reg_,
  // update() ports
  input int update_x
);
/*public:*/

  always_comb begin : update
    update_tock_x = update_x;
    /*update_tock(x);*/
    update_tick_x = update_x;
    /*update_tick(x);*/
  end

  always_comb begin : update_tock
    sig = update_tock_x + 7;
  end
  int update_tock_x;

  always_ff @(posedge clock) begin : update_tick
    reg_ <= update_tick_x + 7;
  end
  int update_tick_x;

endmodule
