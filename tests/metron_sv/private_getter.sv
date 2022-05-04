`include  "metron_tools.sv"

// Private getter methods are OK

module Module
(
  input logic clock,
  output logic[7:0]tock_ret
);
/*public:*/

  function logic[7:0] tock();
    tock = my_getter;
  endfuction
  always_comb tock_ret = tock();

/*private:*/

  function logic[7:0] my_getter();
    my_getter = 12;
  endfuction

endmodule;


