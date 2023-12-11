`include "metron/metron_tools.sv"

module DPRam (
  // global clock
  input logic clock,
  // output registers
  output logic[15:0] porta_dout_,
  output logic[15:0] portb_dout_,
  // tick() ports
  input logic[15:0] tick_porta_addr,
  input logic[15:0] tick_portb_addr,
  input logic tick_portb_write,
  input logic[15:0] tick_portb_data
);
/*public:*/


  always_ff @(posedge clock) begin : tick

    porta_dout_ <= ram_[tick_porta_addr];

    if (tick_portb_write) begin
      ram_[tick_portb_addr] <= tick_portb_data;
      portb_dout_ <= tick_portb_data;
    end
    else begin
      portb_dout_ <= ram_[tick_portb_addr];
    end
  end

/*private:*/
  logic[15:0] ram_[65536];
endmodule
