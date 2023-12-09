`include "metron/metron_tools.sv"

// Public fields that are read by the module but never written become input
// ports.

// this is broken in the new parser because we're not distinguishing
// between input and output signals in submodules...

module Submod (
  // global clock
  input logic clock,
  // input signals
  input logic[7:0] i_signal,
  // output signals
  output logic[7:0] o_signal,
  // output registers
  output logic[7:0] o_reg_,
  // tock() ports
  input logic[7:0] tock_i_param,
  output logic[7:0] tock_ret
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

module Module (
  // global clock
  input logic clock,
  // output registers
  output logic[7:0] my_reg_
);
/*public:*/

  always_comb begin : tock
    logic[7:0] submod_return;
    submod__i_signal = 12;
    submod__tock_i_param = 13;
    submod_return = submod__tock_ret;
    my_sig = submod_return + 3;
    /*tick();*/
  end


/*private:*/

  always_ff @(posedge clock) begin : tick
    my_reg_ <= my_reg_ + my_sig - 2;
  end

  logic[7:0] my_sig;

  Submod submod_(
    // Global clock
    .clock(clock),
    // Input signals
    .i_signal(submod__i_signal),
    // Output signals
    .o_signal(submod__o_signal),
    // Output registers
    .o_reg_(submod__o_reg_),
    // tock() ports
    .tock_i_param(submod__tock_i_param),
    .tock_ret(submod__tock_ret)
  );
  logic[7:0] submod__tock_i_param;
  logic[7:0] submod__tock_ret;
  logic[7:0] submod__i_signal;
  logic[7:0] submod__o_signal;
  logic[7:0] submod__o_reg_;
endmodule
