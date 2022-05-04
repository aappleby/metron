`include  "metron_tools.sv"

// Public register member variables get moved to the output port list.

module Module
(
  input logic clock
);
/*public:*/

  always_comb begin /*tock*/
    /*tick()*/;
  end

/*private:*/

  always_comb begin /*tick*/
    my_reg = 1;
  end

  logic my_reg;
endmodule;


