
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
  output logic o_serial,
  output logic[7:0] o_data,
  output logic o_valid,
  output logic o_done,
  output logic[31:0] o_sum
);
 /*public:*/
  //----------------------------------------

  initial begin : init
    $write("uart_top.init()\n");
    /*hello.init();*/
  end

  always_ff @(posedge clock) begin : tick
    /*rx.tick(i_rstn, tx.o_serial());*/
    /*hello.tick(i_rstn, tx.o_cts(), tx.o_idle());*/
    /*tx.tick(i_rstn, hello.o_data, hello.o_req);*/
  end

  always_comb begin : tock
    /*hello.tock();*/
  end

  always_comb begin o_serial = tx_o_serial; end
  always_comb begin o_data = rx_o_data; end
  always_comb begin o_valid = rx_o_valid; end
  always_comb begin o_done = hello_o_done && tx_o_idle; end
  always_comb begin o_sum = rx_o_sum; end

  //----------------------------------------

 /*private:*/
  uart_hello hello(
    // Inputs
    .clock(clock),
    .i_rstn(i_rstn), 
    .i_cts(tx_o_cts), 
    .i_idle(tx_o_idle), 
    // Outputs
    .o_data(hello_o_data), 
    .o_req(hello_o_req), 
    .o_done(hello_o_done)
  );
  logic[7:0] hello_o_data;
  logic hello_o_req;
  logic hello_o_done;

  uart_tx #(cycles_per_bit) tx(
    // Inputs
    .clock(clock),
    .i_rstn(i_rstn), 
    .i_data(hello_o_data), 
    .i_req(hello_o_req), 
    // Outputs
    .o_serial(tx_o_serial), 
    .o_cts(tx_o_cts), 
    .o_idle(tx_o_idle)
  );
  logic tx_o_serial;
  logic tx_o_cts;
  logic tx_o_idle;

  uart_rx #(cycles_per_bit) rx(
    // Inputs
    .clock(clock),
    .i_rstn(i_rstn), 
    .i_serial(tx_o_serial), 
    // Outputs
    .o_data(rx_o_data), 
    .o_valid(rx_o_valid), 
    .o_sum(rx_o_sum)
  );
  logic[7:0] rx_o_data;
  logic rx_o_valid;
  logic[31:0] rx_o_sum;

endmodule

//==============================================================================

