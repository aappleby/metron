`ifndef UART_TEST_SV
`define UART_TEST_SV
`default_nettype none
`timescale 1 ns / 1 ns

`include "uart_top.sv"

//==============================================================================


module uart_test
#(
  parameter message_file = "override_me1.hex"
);

  // 12 mhz clock
  logic clock;
  logic clock_enable;
  logic reset;
  logic[63:0] timestamp = 0;

  always #5 clock = ~clock & clock_enable;

  always @(posedge clock) begin
    timestamp <= timestamp + 1;
  end

  logic out_serial;
  logic[7:0] out_data;
  logic out_valid;
  logic out_done;
  logic[31:0] out_sum;

  uart_top
  #(
    .cycles_per_bit(3),
    .repeat_msg(0),
    .message_file(message_file)
  )
  top
  (
    .clock(clock),
    .get_serial_ret(out_serial),
    .get_valid_ret(out_valid),
    .get_data_out_ret(out_data),
    .get_done_ret(out_done),
    .get_checksum_ret(out_sum),
    .tock_reset(reset)
  );

  always begin
    wait (!out_valid);
    wait (out_valid);
    $write("%c", out_data);
  end

  initial begin
    $write("Icarus simulation:\n");
    $write("================================================================================\n");

    //$dumpfile("uart_test_iv.vcd");
    //$dumpvars(0, uart_test);

    clock_enable = 0;
    clock = 0;
    reset = 0;
    #5;
    reset = 1;
    #5;
    clock_enable = 1;
    clock = 1;
    #5;
    reset = 0;

    #200;
    wait (top.tx_.get_idle_ret);
    #5

    $write("\n");
    $write("================================================================================\n");
    if (out_sum == 32'h0000b764) $write("All tests pass\n");
    $finish();
  end

endmodule

//==============================================================================

`endif
