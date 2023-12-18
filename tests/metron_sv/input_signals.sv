`include "metron/metron_tools.sv"

// Public fields that are read by the module but never written become input
// ports.

// this is broken in the new parser because we're not distinguishing
// between input and output signals in submodules...

module Submod
(
  // global clock
  input logic clock,
  // input signals
  input logic[7:0] i_signal,
  // output signals
  output logic[7:0] o_signal,
  output logic[7:0] tock_ret,
  // output registers
  output logic[7:0] o_reg_,
  // tock() ports
  input logic[7:0] tock_i_param
);
/*public:*/


  always_comb begin : tock
    o_signal = i_signal + tock_i_param;
    /*tick();*/
    tock_ret = o_signal + 7;
  end

/*private:*/

  always_ff @(posedge clock) begin : tick
    o_reg_ <= o_reg_ + o_signal;
  end
endmodule

module Module
(
  // global clock
  input logic clock,
  // output registers
  output logic[7:0] my_reg_
);
/*public:*/

  always_comb begin : tock
    submod_i_signal = 12;
    submod_tock_i_param = 13;
    /*submod.tock(13);*/
    my_sig = submod_tock_ret + 3;
    /*tick();*/
  end


/*private:*/

  always_ff @(posedge clock) begin : tick
    my_reg_ <= my_reg_ + my_sig - 2;
  end

  logic[7:0] my_sig;

  Submod submod(
    // Global clock
    .clock(clock),
    // Input signals
    .i_signal(submod_i_signal),
    // Output signals
    .o_signal(submod_o_signal),
    .tock_ret(submod_tock_ret),
    // Output registers
    .o_reg_(submod_o_reg_),
    // tock() ports
    .tock_i_param(submod_tock_i_param)
  );
  logic[7:0] submod_tock_i_param;
  logic[7:0] submod_i_signal;
  logic[7:0] submod_o_signal;
  logic[7:0] submod_tock_ret;
  logic[7:0] submod_o_reg_;
endmodule
