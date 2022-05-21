`ifndef UART_RX_H
`define UART_RX_H

`include "metron_tools.sv"

//==============================================================================

module uart_rx
#(parameter int cycles_per_bit = 4)
(
  input logic clock,
  output logic valid_ret,
  output logic[7:0] buffer_ret,
  output logic[31:0] sum_ret,
  input logic tick_i_rstn,
  input logic tick_i_serial
);
/*public:*/
  function logic valid();  valid = _cursor == 1; endfunction
  always_comb valid_ret = valid();
  function logic[7:0] buffer();  buffer = _buffer; endfunction
  always_comb buffer_ret = buffer();
  function logic[31:0] sum();  sum = _sum; endfunction
  always_comb sum_ret = sum();

  always_ff @(posedge clock) begin : tick
    if (!tick_i_rstn) begin
      _cycle <= 0;
      _cursor <= 0;
      _buffer <= 0;
      _sum <= 0;
    end else begin
      if (_cycle != 0) begin
        _cycle <= _cycle - 1;
      end else if (_cursor != 0) begin
        logic[7:0] temp;
        temp = (tick_i_serial << 7) | (_buffer >> 1);
        if (_cursor - 1 == 1) _sum <= _sum + temp;
        _cycle <= cycle_max;
        _cursor <= _cursor - 1;
        _buffer <= temp;
      end else if (tick_i_serial == 0) begin
        _cycle <= cycle_max;
        _cursor <= cursor_max;
      end
    end
  end

  //----------------------------------------
 /*private:*/
  localparam int cycle_bits = $clog2(cycles_per_bit);
  localparam int cycle_max = cycles_per_bit - 1;
  localparam int cursor_max = 9;
  localparam int cursor_bits = $clog2(cursor_max);

  logic[cycle_bits-1:0] _cycle;
  logic[cursor_bits-1:0] _cursor;
  logic[7:0] _buffer;
  logic[31:0] _sum;
endmodule

//==============================================================================

`endif // UART_RX_H
