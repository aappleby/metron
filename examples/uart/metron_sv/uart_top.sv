`ifndef UART_TOP_H
`define UART_TOP_H

`include "metron/metron_tools.sv"
`include "uart_hello.sv"
`include "uart_rx.sv"
`include "uart_tx.sv"

//==============================================================================

module uart_top
#(
  parameter cycles_per_bit = 3,
  parameter repeat_msg = 0
)
(
  // global clock
  input logic clock,
  // get_serial() ports
  output logic get_serial_ret,
  // get_valid() ports
  output logic get_valid_ret,
  // get_data_out() ports
  output logic[7:0] get_data_out_ret,
  // get_done() ports
  output logic get_done_ret,
  // get_checksum() ports
  output logic[31:0] get_checksum_ret,
  // tock() ports
  input logic tock_reset
);
/*public:*/

  // The actual bit of data currently on the transmitter's output
  always_comb begin : get_serial
    get_serial_ret = tx__get_serial_ret;
  end

  // Returns true if the receiver has a byte in its buffer
  always_comb begin : get_valid
    get_valid_ret = rx__get_valid_ret;
  end

  // The next byte of data from the receiver
  always_comb begin : get_data_out
    get_data_out_ret = rx__get_data_out_ret;
  end

  // True if the client has sent its message and the transmitter has finished
  // transmitting it.
  always_comb begin : get_done
    get_done_ret = hello__get_done_ret && tx__get_idle_ret;
  end

  // Checksum of all the bytes received
  always_comb begin : get_checksum
    get_checksum_ret = rx__get_checksum_ret;
  end

  always_comb begin : tock
    logic[7:0] data;
    logic request;
    logic serial;
    logic clear_to_send;
    logic idle;
    // Grab signals from our submodules before we tick them.
    data = hello__get_data_ret;
    request = hello__get_request_ret;

    serial = tx__get_serial_ret;
    clear_to_send = tx__get_clear_to_send_ret;
    idle = tx__get_idle_ret;

    // Tick all submodules.
    hello__tick_reset = tock_reset;
    hello__tick_clear_to_send = clear_to_send;
    hello__tick_idle = idle;
    /*hello_.tick(reset, clear_to_send, idle);*/
    tx__tick_reset = tock_reset;
    tx__tick_send_data = data;
    tx__tick_send_request = request;
    /*tx_.tick(reset, data, request);*/
    rx__tick_reset = tock_reset;
    rx__tick_serial = serial;
    /*rx_.tick(reset, serial);*/
  end

  //----------------------------------------
/*private:*/
  // Our UART client that transmits our "hello world" test message
  uart_hello  #(
    // Template Parameters
    .repeat_msg(repeat_msg)
  ) hello_(
    // Global clock
    .clock(clock),
    // get_data() ports
    .get_data_ret(hello__get_data_ret),
    // get_request() ports
    .get_request_ret(hello__get_request_ret),
    // get_done() ports
    .get_done_ret(hello__get_done_ret),
    // tick() ports
    .tick_reset(hello__tick_reset),
    .tick_clear_to_send(hello__tick_clear_to_send),
    .tick_idle(hello__tick_idle)
  );
  logic[7:0] hello__get_data_ret;
  logic hello__get_request_ret;
  logic hello__get_done_ret;
  logic hello__tick_reset;
  logic hello__tick_clear_to_send;
  logic hello__tick_idle;
  // The UART transmitter
  uart_tx #(
    // Template Parameters
    .cycles_per_bit(cycles_per_bit)
  ) tx_(
    // Global clock
    .clock(clock),
    // get_serial() ports
    .get_serial_ret(tx__get_serial_ret),
    // get_clear_to_send() ports
    .get_clear_to_send_ret(tx__get_clear_to_send_ret),
    // get_idle() ports
    .get_idle_ret(tx__get_idle_ret),
    // tick() ports
    .tick_reset(tx__tick_reset),
    .tick_send_data(tx__tick_send_data),
    .tick_send_request(tx__tick_send_request)
  );
  logic tx__get_serial_ret;
  logic tx__get_clear_to_send_ret;
  logic tx__get_idle_ret;
  logic tx__tick_reset;
  logic[7:0] tx__tick_send_data;
  logic tx__tick_send_request;
  // The UART receiver
  uart_rx #(
    // Template Parameters
    .cycles_per_bit(cycles_per_bit)
  ) rx_(
    // Global clock
    .clock(clock),
    // get_valid() ports
    .get_valid_ret(rx__get_valid_ret),
    // get_data_out() ports
    .get_data_out_ret(rx__get_data_out_ret),
    // get_checksum() ports
    .get_checksum_ret(rx__get_checksum_ret),
    // tick() ports
    .tick_reset(rx__tick_reset),
    .tick_serial(rx__tick_serial)
  );
  logic rx__get_valid_ret;
  logic[7:0] rx__get_data_out_ret;
  logic[31:0] rx__get_checksum_ret;
  logic rx__tick_reset;
  logic rx__tick_serial;
endmodule

//==============================================================================

`endif // UART_TOP_H
