`include "metron/tools/metron_tools.sv"

// Modules can use constructors to initialize state

module Module (
  // get_reg() ports
  output logic[7:0] get_reg_ret
);
/*public:*/

  initial begin
    my_reg = 7;
  end

  always_comb begin : get_reg
    get_reg_ret = my_reg;
  end

/*private:*/

  logic[7:0] my_reg;
endmodule;