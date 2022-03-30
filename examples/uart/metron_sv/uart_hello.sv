
`include "metron_tools.sv"

//==============================================================================

module uart_hello
(
  input logic clock,
  input logic i_rstn,
  input logic i_cts,
  input logic i_idle,
  output logic[7:0] o_data,
  output logic o_req,
  output logic o_done
);
 /*public:*/
  initial begin : init $readmemh("examples/uart/message.hex", memory, 0, 511); end

  //----------------------------------------

  always_comb begin o_data = data; end
  always_comb begin o_req = s == SEND; end
  always_comb begin o_done = s == DONE; end

  always_ff @(posedge clock) begin : tick
    if (!i_rstn) begin
      s <= WAIT;
      cursor <= 0;
    end else begin
      data <= memory[cursor];
      if (s == WAIT && i_idle) begin
        s <= SEND;
      end else if (s == SEND && i_cts) begin
        if (cursor == 9'(message_len - 1)) begin
          s <= DONE;
        end else begin
          cursor <= cursor + 1;
        end
      end else if (s == DONE) begin
        // s = state::WAIT;
        cursor <= 0;
      end
    end
  end

  //----------------------------------------

 /*private:*/
  localparam /*const*/ int message_len = 512;
  localparam /*const*/ int cursor_bits = $clog2(message_len);

  typedef enum logic[1:0] {WAIT, SEND, DONE} state;

  state s;
  logic[cursor_bits-1:0] cursor;
  logic[7:0] memory[512];
  logic[7:0] data;
endmodule

//==============================================================================

