`include "metron/tools/metron_tools.sv"

// Nesting submod calls should put all the nested input bindings above the call.

module Submod1 (
  // add_one() ports
  input logic[7:0] add_one_a,
  output logic[7:0] add_one_ret
);
/*public:*/
  always_comb begin : add_one
    add_one_ret = add_one_a + 1;
  end
endmodule

module Submod2 (
  // add_two() ports
  input logic[7:0] add_two_a,
  output logic[7:0] add_two_ret
);
/*public:*/
  always_comb begin : add_two
    add_two_ret = add_two_a + 1;
  end
endmodule

module Module (
  // tock() ports
  input logic[7:0] tock_old_counter,
  output logic[7:0] tock_ret
);
/*public:*/

  always_comb begin : tock
    logic[7:0] new_counter;
    // Two bindings should end up here.
    submod2_add_two_a = tock_old_counter;
    submod1_add_one_a = submod2_add_two_ret;
    new_counter = submod1_add_one_ret;
    tock_ret = new_counter;
  end

/*private:*/

  Submod1  submod1(
  );
  (submod binding fields go here);
  Submod2  submod2(
  );
  (submod binding fields go here);
endmodule
