`include "metron/metron_tools.sv"

// Members whose names have trailing underscores are states.
// Members without trailing underscores are signals.
// We should not require any other special annotations on our modules.

module Module
(
  // global clock
  input logic clock,
  // get_even() ports
  output logic get_even_ret,
  // tock() ports
  input logic[31:0] tock_inc,
  // get_counter() ports
  output logic[31:0] get_counter_ret
);
/*public:*/

  initial begin
    counter_ = 0;
  end

  always_comb begin : get_even
    get_even_ret = is_even;
  end

  always_comb begin : tock
    is_even = (counter_ & 1) == 0;
    update_inc = tock_inc;
    /*update(inc);*/
  end

  always_ff @(posedge clock) begin : update
    update2(update_inc);
  end
  logic[31:0] update_inc;

  task automatic update2(logic[31:0] inc);
    counter_ <= counter_ + inc;
  endtask

  always_comb begin : get_counter
    get_counter_ret = counter_;
  end

/*private:*/

  logic  is_even;
  logic[31:0] counter_;
endmodule
