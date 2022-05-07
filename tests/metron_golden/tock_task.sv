module Module
(
  input logic clock,
  output logic[7:0] my_signal
);
  logic[7:0] my_reg;

  function logic[7:0] get_number();
    get_number = 7;
  endfunction

  function void set_signal(input logic[7:0] number);
    my_signal = number;
  endfunction

  function void tock();
    set_signal(get_number());
  endfunction

  function void tick();
    my_reg <= my_reg + 1;
  endfunction

  always_comb begin
    tock();
  end

  always_ff @(posedge clock) begin
    tick();
  end

endmodule
