`ifndef UART_TOP_H
`define UART_TOP_H

`include "metron_tools.sv"
`include "uart_hello.sv"
`include "uart_rx.sv"
`include "uart_tx.sv"

//==============================================================================

module uart_top #(parameter int cycles_per_bit = 3,parameter  int repeat_msg = 0)
(
  // global clock
  input logic clock,
  // serial() ports
  output logic serial_ret,
  // data() ports
  output logic[7:0] data_ret,
  // valid() ports
  output logic valid_ret,
  // done() ports
  output logic done_ret,
  // sum() ports
  output logic[31:0] sum_ret,
  // tock() ports
  input logic tock_i_rstn,
  output logic tock_ret
);
/*public:*/
  always_comb begin : serial
    serial_ret = tx_serial_ret;
  end

  always_comb begin : data
    data_ret = rx_buffer_ret;
  end

  always_comb begin : valid
    valid_ret = rx_valid_ret;
  end

  always_comb begin : done
    done_ret = hello_done_ret && tx_idle_ret;
  end

  always_comb begin : sum
    sum_ret = rx_sum_ret;
  end

  always_comb begin : tock
    logic[7:0] hello_data;
    logic hello_req;
    hello_data = hello_data_ret;
    hello_req = hello_req_ret;
    rx_tick_i_rstn = tock_i_rstn;
    rx_tick_i_serial = tx_serial_ret;

    hello_tick_i_rstn = tock_i_rstn;
    hello_tick_i_cts = tx_cts_ret;
    hello_tick_i_idle = tx_idle_ret;
    tx_tick_i_rstn = tock_i_rstn;
    tx_tick_i_data = hello_data;
    tx_tick_i_req = hello_req;
    tock_ret = 0;
  end

  //----------------------------------------
/*private:*/
  uart_hello #(repeat_msg) hello(
    // global clock
    .clock(clock),
    // data() ports
    .data_ret(hello_data_ret),
    // req() ports
    .req_ret(hello_req_ret),
    // done() ports
    .done_ret(hello_done_ret),
    // tick() ports
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
    // global clock
    .clock(clock),
    // serial() ports
    .serial_ret(tx_serial_ret),
    // cts() ports
    .cts_ret(tx_cts_ret),
    // idle() ports
    .idle_ret(tx_idle_ret),
    // tick() ports
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
    // global clock
    .clock(clock),
    // valid() ports
    .valid_ret(rx_valid_ret),
    // buffer() ports
    .buffer_ret(rx_buffer_ret),
    // sum() ports
    .sum_ret(rx_sum_ret),
    // tick() ports
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
