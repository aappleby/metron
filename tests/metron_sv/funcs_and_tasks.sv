`include "metron_tools.sv"

// All the combos of tasks/funcs should work from ticks and tocks.

module Module
(
  input logic clock,
  output logic[31:0] my_sig,
  output logic done,
  output logic[31:0] result
);
/*public:*/
  initial begin /*Module*/
    my_sig = 0;
    my_reg = 0;
    counter = 0;
  end


  always_comb begin /*done*/
    done = counter > 1000;
  end

  always_comb begin /*result*/
    result = my_reg;
  end

  always_comb begin /*tock*/
    public_task(public_func(171));
    /*tick()*/;
  end

  task public_task(logic[31:0] public_task_x);
    my_sig = public_task_x + 7;
  endtask

  function logic[31:0] public_func(logic[7:0] public_func_x);
    public_func = my_reg + private_func(51);
  endfunction

/*private:*/

  always_ff @(posedge clock) begin /*tick*/
    private_task(private_func(33));
    counter <= counter + 1;
  end

  task private_task(logic[31:0] private_task_x);
    my_reg = my_reg - private_func(17);
  endtask

  function logic[31:0] private_func(logic[31:0] private_func_y);
    private_func = (my_reg >> 3) + private_func_y;
  endfunction

  logic[31:0] my_reg;
  logic[31:0] counter;
endmodule

