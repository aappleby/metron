
`include "metron_tools.sv"
`include "uart_hello.sv"
`include "uart_rx.sv"
`include "uart_tx.sv"

//==============================================================================

module uart_top
#(parameter int cycles_per_bit = 3)
(
  input logic clock,
  input logic i_rstn,
  output logic[7:0]  o_data,
  output logic  o_valid,
  output logic  o_done,
  output logic[31:0] o_sum
);
 /*public:*/
  initial begin : init
    $write("uart_top.init()\n");
    /*hello.init();*/
  end

  //----------------------------------------

  always_comb begin o_data = rx_o_buffer; end
  always_comb begin o_valid = rx_o_valid; end
  always_comb begin o_done = hello_o_done && tx_o_idle; end
  always_comb begin o_sum = rx_o_sum; end

  always_comb begin : tock_update
    logic[7:0] hello_data;
    logic hello_req;
    hello_data = hello_o_data;
    hello_req = hello_o_req;

    /*rx.tick(i_rstn, tx.o_serial());*/
    rx_i_rstn = i_rstn;
    rx_i_serial = tx_o_serial;
    /*hello.tick(i_rstn, tx.o_cts(), tx.o_idle());*/
    hello_i_rstn = i_rstn;
    hello_i_cts = tx_o_cts;
    hello_i_idle = tx_o_idle;
    /*tx.tick(i_rstn, hello_data, hello_req);*/
    tx_i_rstn = i_rstn;
    tx_i_data = hello_data;
    tx_i_req = hello_req;
  end

  //----------------------------------------

 /*private:*/
  uart_hello hello(
    // Inputs
    .clock(clock),
    .i_rstn(hello_i_rstn), 
    .i_cts(hello_i_cts), 
    .i_idle(hello_i_idle), 
    // Outputs
    .o_data(hello_o_data), 
    .o_req(hello_o_req), 
    .o_done(hello_o_done)
  );
  logic hello_i_rstn;
  logic hello_i_cts;
  logic hello_i_idle;
  logic[7:0] hello_o_data;
  logic hello_o_req;
  logic hello_o_done;

  uart_tx #(cycles_per_bit) tx(
    // Inputs
    .clock(clock),
    .i_rstn(tx_i_rstn), 
    .i_data(tx_i_data), 
    .i_req(tx_i_req), 
    // Outputs
    .o_serial(tx_o_serial), 
    .o_cts(tx_o_cts), 
    .o_idle(tx_o_idle)
  );
  logic tx_i_rstn;
  logic[7:0] tx_i_data;
  logic tx_i_req;
  logic tx_o_serial;
  logic tx_o_cts;
  logic tx_o_idle;

  uart_rx #(cycles_per_bit) rx(
    // Inputs
    .clock(clock),
    .i_rstn(rx_i_rstn), 
    .i_serial(rx_i_serial), 
    // Outputs
    .o_valid(rx_o_valid), 
    .o_buffer(rx_o_buffer), 
    .o_sum(rx_o_sum)
  );
  logic rx_i_rstn;
  logic rx_i_serial;
  logic  rx_o_valid;
  logic[7:0]  rx_o_buffer;
  logic[31:0] rx_o_sum;

endmodule

//==============================================================================

