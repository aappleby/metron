`include "metron_tools.sv"

//==============================================================================

module uart_rx
#(parameter int cycles_per_bit = 4)
(
  input logic clock,
  input logic i_rstn,
  input logic i_serial,
  output logic o_valid,
  output logic[7:0] o_buffer,
  output logic[31:0] o_sum
);
  //----------------------------------------

  // yosys doesn't appear to handle return values from functions at all
  // v*rilator doesn't allow "return;" if the function has a return value
  // even if you set it via "func_name = value;" first.


  always_comb begin o_valid = cursor == 1; end
  always_comb begin o_buffer = buffer; end
  always_comb begin o_sum = sum; end

  always_comb begin /*tock*/ /*tick(i_rstn, i_serial)*/; end

  //----------------------------------------
  task tick();
    if (!i_rstn) begin
      cycle <= 0;
      cursor <= 0;
      buffer <= 0;
      sum <= 0;
    end else begin
      if (cycle != 0) begin
        cycle <= cycle - 1;
      end else if (cursor != 0) begin
        logic[7:0] temp;

        temp = (i_serial << 7) | (buffer >> 1);
        if (cursor - 1 == 1) sum <= sum + temp;
        cycle <= cycle_max;
        cursor <= cursor - 1;
        buffer <= temp;
      end else if (i_serial == 0) begin
        cycle <= cycle_max;
        cursor <= cursor_max;
      end
    end
  endtask
  always_ff @(posedge clock) tick();

  localparam /*const*/ int cycle_bits = $clog2(cycles_per_bit);
  localparam /*const*/ int cycle_max = cycles_per_bit - 1;
  localparam /*const*/ int cursor_max = 9;
  localparam /*const*/ int cursor_bits = $clog2(cursor_max);

  logic[cycle_bits-1:0] cycle;
  logic[cursor_bits-1:0] cursor;
  logic[7:0] buffer;
  logic[31:0] sum;
endmodule

//==============================================================================

