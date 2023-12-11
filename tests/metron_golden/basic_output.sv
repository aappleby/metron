`include "metron/metron_tools.sv"

// Getter methods should turn into outputs.

module Module (
  // global clock
  input logic clock,
  // get_reg() ports
  output logic[6:0] get_reg_ret
);
/*public:*/

  always_comb begin : get_reg
    get_reg_ret = my_reg_;
  end

  always_ff @(posedge clock) begin : tock
    tick();
  end


/*private:*/

  task automatic tick();
    my_reg_ <= my_reg_ + 1;
  endtask

  logic[6:0] my_reg_;
endmodule
