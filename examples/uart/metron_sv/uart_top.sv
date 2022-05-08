`ifndef UART_TOP_H
`define UART_TOP_H

`include "metron_tools.sv"
`include "uart_hello.sv"
`include "uart_rx.sv"
`include "uart_tx.sv"

//==============================================================================

module uart_top
#(parameter int cycles_per_bit = 3,parameter  int repeat_msg = 0)
(
  input logic clock,
  output logic serial_ret,
  output logic[7:0] data_ret,
  output logic valid_ret,
  output logic done_ret,
  output logic[31:0] sum_ret,
  input logic tock_i_rstn
);
 /*public:*/
  function logic serial();  serial = tx_serial_ret; endfunction
  always_comb serial_ret = serial();
  function logic[7:0] data();  data = rx_buffer_ret; endfunction
  always_comb data_ret = data();
  function logic valid();  valid = rx_valid_ret; endfunction
  always_comb valid_ret = valid();
  function logic done();  done = hello_done_ret && tx_idle_ret; endfunction
  always_comb done_ret = done();
  function logic[31:0] sum();  sum = rx_sum_ret; endfunction
  always_comb sum_ret = sum();

  task automatic tock(logic i_rstn);
    logic[7:0] hello_data;
    logic hello_req;
    hello_data = hello_data_ret;
    hello_req = hello_req_ret;
    rx_tick_i_rstn = tock_i_rstn;
    rx_tick_i_serial = tx_serial_ret;

    /*rx.tick*/;
    hello_tick_i_rstn = tock_i_rstn;
    hello_tick_i_cts = tx_cts_ret;
    hello_tick_i_idle = tx_idle_ret;
    /*hello.tick*/;
    tx_tick_i_rstn = tock_i_rstn;
    tx_tick_i_data = hello_data;
    tx_tick_i_req = hello_req;
    /*tx.tick*/;
  endtask
  always_comb tock(tock_i_rstn);

  //----------------------------------------
 /*private:*/
  uart_hello #(repeat_msg) hello(
    .clock(clock),
    .data_ret(hello_data_ret),
    .req_ret(hello_req_ret),
    .done_ret(hello_done_ret),
    .tick_i_rstn(hello_tick_i_rstn),
    .tick_i_cts(hello_tick_i_cts),
    .tick_i_idle(hello_tick_i_idle)
  );
  logic hello_tick_i_rstn;
  logic hello_tick_i_cts;
  logic hello_tick_i_idle;
  logic[7:0] hello_data_ret;
  logic hello_req_ret;
  logic hello_done_ret;

  uart_tx #(cycles_per_bit) tx(
    .clock(clock),
    .serial_ret(tx_serial_ret),
    .cts_ret(tx_cts_ret),
    .idle_ret(tx_idle_ret),
    .tick_i_rstn(tx_tick_i_rstn),
    .tick_i_data(tx_tick_i_data),
    .tick_i_req(tx_tick_i_req)
  );
  logic tx_tick_i_rstn;
  logic[7:0] tx_tick_i_data;
  logic tx_tick_i_req;
  logic tx_serial_ret;
  logic tx_cts_ret;
  logic tx_idle_ret;

  uart_rx #(cycles_per_bit) rx(
    .clock(clock),
    .valid_ret(rx_valid_ret),
    .buffer_ret(rx_buffer_ret),
    .sum_ret(rx_sum_ret),
    .tick_i_rstn(rx_tick_i_rstn),
    .tick_i_serial(rx_tick_i_serial)
  );
  logic rx_tick_i_rstn;
  logic rx_tick_i_serial;
  logic rx_valid_ret;
  logic[7:0] rx_buffer_ret;
  logic[31:0] rx_sum_ret;

endmodule

//==============================================================================

`endif // UART_TOP_H
