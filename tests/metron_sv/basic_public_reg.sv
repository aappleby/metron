`include "metron/metron_tools.sv"

// Public register member variables get moved to the output port list.

module Module (
  // global clock
  input logic clock,
  // output registers
  output logic my_reg_
);
/*public:*/
  always_ff @(posedge clock) begin : tock
    tick();
  end

/*private:*/
  task automatic tick();
    my_reg_ <= my_reg_ + 1;
  endtask
endmodule
