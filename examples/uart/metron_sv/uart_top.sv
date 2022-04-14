`include "metron_tools.sv"
`include "uart_hello.sv"
`include "uart_rx.sv"
`include "uart_tx.sv"

//==============================================================================

module uart_top
#(parameter int cycles_per_bit = 3, parameter int repeat_msg = 0)
(
  input logic clock,
  input logic tock_i_rstn,
  output logic  tock_serial,
  output logic[7:0]  tock_data,
  output logic  tock_valid,
  output logic  tock_done,
  output logic[31:0] tock_sum
);
 /*public:*/
  initial begin /*uart_top*/
    $write("uart_top.init()\n");
  end

  //----------------------------------------

  always_comb begin /*tock_serial*/ tock_serial = tx_tock_serial; end
  always_comb   begin /*tock_data*/ tock_data = rx_tock_buffer; end
  always_comb  begin /*tock_valid*/ tock_valid = rx_tock_valid; end
  always_comb   begin /*tock_done*/ tock_done = hello_tock_done && tx_tock_idle; end
  always_comb    begin /*tock_sum*/ tock_sum = rx_tock_sum; end

  always_comb begin /*tock*/
    logic[7:0] hello_data;
    logic hello_req;

    hello_data = hello_tock_data;
    hello_req = hello_tock_req;

    rx_tock_i_rstn = tock_i_rstn;
    rx_tock_i_serial = tx_tock_serial;
    /*rx.tock(i_rstn, tx.tock_serial())*/;
    hello_tock_i_rstn = tock_i_rstn;
    hello_tock_i_cts = tx_tock_cts;
    hello_tock_i_idle = tx_tock_idle;
    /*hello.tock(i_rstn, tx.tock_cts(), tx.tock_idle())*/;
    tx_tock_i_rstn = tock_i_rstn;
    tx_tock_i_data = hello_data;
    tx_tock_i_req = hello_req;
    /*tx.tock(i_rstn, hello_data, hello_req)*/;
  end

  //----------------------------------------

 /*private:*/
  uart_hello #(repeat_msg) hello(
    // Inputs
    .clock(clock),
    .tock_i_rstn(hello_tock_i_rstn),
    .tock_i_cts(hello_tock_i_cts),
    .tock_i_idle(hello_tock_i_idle),
    // Outputs
    .tock_data(hello_tock_data),
    .tock_req(hello_tock_req),
    .tock_done(hello_tock_done)
  );
  logic hello_tock_i_rstn;
  logic hello_tock_i_cts;
  logic hello_tock_i_idle;
  logic[7:0] hello_tock_data;
  logic hello_tock_req;
  logic hello_tock_done;

  uart_tx #(cycles_per_bit) tx(
    // Inputs
    .clock(clock),
    .tock_i_rstn(tx_tock_i_rstn),
    .tock_i_data(tx_tock_i_data),
    .tock_i_req(tx_tock_i_req),
    // Outputs
    .tock_serial(tx_tock_serial),
    .tock_cts(tx_tock_cts),
    .tock_idle(tx_tock_idle)
  );
  logic tx_tock_i_rstn;
  logic[7:0] tx_tock_i_data;
  logic tx_tock_i_req;
  logic tx_tock_serial;
  logic tx_tock_cts;
  logic tx_tock_idle;

  uart_rx #(cycles_per_bit) rx(
    // Inputs
    .clock(clock),
    .tock_i_rstn(rx_tock_i_rstn),
    .tock_i_serial(rx_tock_i_serial),
    // Outputs
    .tock_valid(rx_tock_valid),
    .tock_buffer(rx_tock_buffer),
    .tock_sum(rx_tock_sum)
  );
  logic rx_tock_i_rstn;
  logic rx_tock_i_serial;
  logic  rx_tock_valid;
  logic[7:0]  rx_tock_buffer;
  logic[31:0] rx_tock_sum;

endmodule

//==============================================================================

