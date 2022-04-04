`include "metron_tools.sv"

// Multiple tick methods are OK as long as they're all called by tock()

module Module
(
  input logic clock
);
/*public:*/

  always_comb begin /*tock*/
    /*tick1()*/;
    /*tick2()*/;
  end

/*private:*/

  task tick1(); 
    my_reg1 <= 0;
  endtask
  always_ff @(posedge clock) tick1();

  task tick2(); 
    my_reg2 <= 1;
  endtask
  always_ff @(posedge clock) tick2();

  logic my_reg1;
  logic my_reg2;
endmodule

