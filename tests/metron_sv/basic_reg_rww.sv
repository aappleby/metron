`include "metron/metron_tools.sv"

// Writing a register multiple times in the same function is OK.

module Module (
  // global clock
  input logic clock
);
/*public:*/

  always_ff @(posedge clock) begin : tock
    tick();
  end

/*private:*/

  task automatic tick();
    logic temp;
    temp = my_reg_;
    my_reg_ <= 0;
    my_reg_ <= 1;
  endtask

  logic my_reg_;
endmodule
