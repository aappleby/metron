`ifndef UART_RX_H
`define UART_RX_H

`include "metron_tools.sv"

//==============================================================================

module uart_rx #(parameter int cycles_per_bit = 4)
(
  // global clock
  input logic clock,
  // valid() ports
  output logic valid_ret,
  // buffer() ports
  output logic[7:0] buffer_ret,
  // sum() ports
  output logic[31:0] sum_ret,
  // tick() ports
  input logic tick_i_rstn,
  input logic tick_i_serial
);
/*public:*/
  always_comb begin : valid
    valid_ret = _cursor == 1;
  end

  always_comb begin : buffer
    buffer_ret = _buffer;
  end

  always_comb begin : sum
    sum_ret = _sum;
  end

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
