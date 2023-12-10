`ifndef UART_HELLO_H
`define UART_HELLO_H

`include "metron/metron_tools.sv"

//==============================================================================

module uart_hello (
  // global clock
  input logic clock,
  // get_data() ports
  output logic[7:0] get_data_ret,
  // get_request() ports
  output logic get_request_ret,
  // get_done() ports
  output logic get_done_ret,
  // tick() ports
  input logic tick_reset,
  input logic tick_clear_to_send,
  input logic tick_idle
);
  parameter repeat_msg = 0;
/*public:*/
  initial begin
    $readmemh("examples/uart/message.hex", memory_, 0, 511);
    state_ = 0;
    cursor_ = 0;
  end

  // The byte of data we want transmitted is always the one at the cursor.
  always_comb begin : get_data
    // FIXME why aren't we seeing "Directly reads uart_hello.memory_"?
    get_data_ret = memory_[cursor_];
  end

  // True if we want to transmit a byte
  always_comb begin : get_request
    get_request_ret = state_ == SEND;
  end

  // True if we've transmitted the whole message.
  always_comb begin : get_done
    get_done_ret = state_ == DONE;
  end

  always_ff @(posedge clock) begin : tick           // True if the transmitter is idle

    // In reset we're always in WAIT state with the message cursor set to
    // the start of the message buffer.
    if (tick_reset) begin
      state_ <= WAIT;
      cursor_ <= 0;
    end
    else begin
      // If we're waiting for the transmitter to be free and it's told us that
      // it's idle, go to SEND state.
      if (state_ == WAIT && tick_idle) begin
        state_ <= SEND;
      end

      // If we're currently sending a message and the transmitter is ready to
      // accept another byte,
      else if (state_ == SEND && tick_clear_to_send) begin
        // either go to DONE state if we're about to send the last character of
        // the message
        if (cursor_ == message_len - 1) begin
          state_ <= DONE;
        end

        // or just advance the message cursor.
        else begin
          cursor_ <= cursor_ + 1;
        end
      end

      // If we've finished transmitting, reset the message cursor and either go
      // back to WAIT state if we want to re-transmit or just stay in DONE
      // otherwise.
      else if (state_ == DONE) begin
        cursor_ <= 0;
        if (repeat_msg) state_ <= WAIT;
      end
    end
  end

/*private:*/
  localparam /*static*/ /*const*/ int message_len = 512;
  localparam /*static*/ /*const*/ int cursor_bits = $clog2(message_len);

  localparam /*static*/ /*const*/ int WAIT = 0; // Waiting for the transmitter to be free
  localparam /*static*/ /*const*/ int SEND = 1; // Sending the message buffer
  localparam /*static*/ /*const*/ int DONE = 2; // Message buffer sent
  logic[1:0] state_;            // One of the above states

  logic[7:0] memory_[512];      // The buffer preloaded with our message
  logic[cursor_bits-1:0] cursor_; // Index into the message buffer of the _next_ character to transmit
endmodule

//==============================================================================

`endif // UART_HELLO_H
