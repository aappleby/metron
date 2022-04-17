`include "metron_tools.sv"

// Calling a tock() from a tick() is not allowed.

//X Can't call tock2 from tick method tick

module Module
(
  input logic clock
);
/*public:*/

  always_comb begin /*tock1*/
    /*tick()*/;
  end

  task tock2();
    sig = 1;
  endtask

/*private:*/

  always_ff @(posedge clock) begin /*tick*/
    reg <= 0;
    tock2();
  end

  logic reg;
  logic sig;
endmodule

