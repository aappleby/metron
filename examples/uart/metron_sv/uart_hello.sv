`include "metron_tools.sv"

//==============================================================================

module uart_hello
#(parameter int repeat_msg = 0)
(
  input logic clock,
  input logic i_rstn,
  input logic i_cts,
  input logic i_idle,
  output logic[7:0] o_data,
  output logic o_req,
  output logic o_done
);
  initial begin /*uart_hello*/
    $readmemh("examples/uart/message.hex", memory, 0, 511);
  end

  //----------------------------------------

  always_comb begin o_data = data; end
  always_comb begin o_req = state == SEND; end
  always_comb begin o_done = state == DONE; end

  always_comb begin /*tock*/
    /*tick(i_rstn, i_cts, i_idle)*/;
  end

  //----------------------------------------

  task tick();
    if (!i_rstn) begin
      state <= WAIT;
      cursor <= 0;
    end else begin
      data <= memory[cursor];
      if (state == WAIT && i_idle) begin
        state <= SEND;
      end else if (state == SEND && i_cts) begin
        if (cursor == 9'(message_len - 1)) begin
          state <= DONE;
        end else begin
          cursor <= cursor + 1;
        end
      end else if (state == DONE) begin
        if (repeat_msg) state <= WAIT;
        cursor <= 0;
      end
    end
  endtask
  always_ff @(posedge clock) tick();

  localparam /*const*/ int message_len = 512;
  localparam /*const*/ int cursor_bits = $clog2(message_len);

  localparam /*const*/ int WAIT = 0;
  localparam /*const*/ int SEND = 1;
  localparam /*const*/ int DONE = 2;

  //enum class state : logic<2>::BASE{WAIT, SEND, DONE};

  logic[1:0] state;
  logic[cursor_bits-1:0] cursor;
  logic[7:0] memory[512];
  logic[7:0] data;
endmodule

//==============================================================================

