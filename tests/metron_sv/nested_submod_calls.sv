`include "metron_tools.sv"

// Nesting submod calls should put all the nested input bindings above the call.

module Submod1
(
  input logic[7:0] add_one_a,
  output logic[7:0] add_one_ret
);
/*public:*/
  function logic[7:0] add_one(logic[7:0] a);
    add_one = a + 1;
  endfunction
  always_comb add_one_ret = add_one(add_one_a);
endmodule

module Submod2
(
  input logic[7:0] add_two_a,
  output logic[7:0] add_two_ret
);
/*public:*/
  function logic[7:0] add_two(logic[7:0] a);
    add_two = a + 1;
  endfunction
  always_comb add_two_ret = add_two(add_two_a);
endmodule

module Module
(
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

  Submod1 submod1(
    .add_one_a(submod1_add_one_a),
    .add_one_ret(submod1_add_one_ret)
  );
  logic[7:0] submod1_add_one_a;
  logic[7:0] submod1_add_one_ret;

  Submod2 submod2(
    .add_two_a(submod2_add_two_a),
    .add_two_ret(submod2_add_two_ret)
  );
  logic[7:0] submod2_add_two_a;
  logic[7:0] submod2_add_two_ret;

endmodule
