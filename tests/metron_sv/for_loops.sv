`include "metron_tools.sv"

module Module
(
  // loop1() ports
  output int loop1_ret,
  // loop2() ports
  output int loop2_ret
);
  /*public:*/

  // decl in for
  always_comb begin : loop1
    int x;
    int i;
    x = 0;
    for (i = 0; i < 10; i = i + 1) begin
      x = x + i;
    end
    loop1_ret = x;
  end

  // decl outside of for
  always_comb begin : loop2
    int x;
    int i;
    x = 0;
    for (i = 0; i < 10; i = i + 1) begin
      x = x + i;
    end
    loop2_ret = x;
  end
endmodule
