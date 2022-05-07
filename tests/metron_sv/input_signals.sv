`include "metron_tools.sv"

// Public fields that are read by the module but never written become input
// ports.

module Submod
(
  input logic clock,
  output logic[7:0] i_signal,
  output logic[7:0] o_signal,
  output logic[7:0] o_reg,
  input logic[7:0] tock_i_param,
  output logic[7:0] tock_ret
);
/*public:*/


  function logic[7:0] tock(logic[7:0] i_param);
    o_signal = i_signal + i_param;
    /*tick()*/;
    tock = o_signal + 7;
  endfunction

/*private:*/

  function void tick();
    o_reg <= o_reg + o_signal;
  endfunction

  //----------------------------------------
  always_comb begin
    tock_ret = tock(tock_i_param);
  end


  always_ff @(posedge clock) begin
    tick();
  end

endmodule

module Module
(
  input logic clock,
  output logic[7:0] my_reg
);
/*public:*/

  function void tock();
    logic[7:0] submod_return;
    submod_i_signal = 12;    submod_tock_i_param = 13;

    submod_return = submod_tock_ret;
    my_sig = submod_return + 3;
    tick();
  endfunction


/*private:*/

  function void tick();
    my_reg = my_sig - 2;
  endfunction

  logic[7:0] my_sig;

  Submod submod(
    .clock(clock),
    .i_signal(submod_i_signal),
    .o_signal(submod_o_signal),
    .o_reg(submod_o_reg),
    .tock_i_param(submod_tock_i_param),
    .tock_ret(submod_tock_ret)
  );
  logic[7:0] submod_tock_i_param;
  logic[7:0] submod_i_signal;
  logic[7:0] submod_o_signal;
  logic[7:0] submod_o_reg;
  logic[7:0] submod_tock_ret;


  //----------------------------------------
  always_comb begin
    tock();
  end

endmodule
