`include "metron/metron_tools.sv"

module DPRam (
  // output signals
  output logic[15:0] porta_dout,
  output logic[15:0] portb_dout,
  // tick() ports
  input logic[15:0] tick_porta_addr,
  input logic[15:0] tick_portb_addr,
  input logic tick_portb_write,
  input logic[15:0] tick_portb_data
);
/*public:*/


  always_comb begin : tick

    porta_dout = ram[tick_porta_addr];

    if (tick_portb_write) begin
      ram[tick_portb_addr] = tick_portb_data;
      portb_dout = tick_portb_data;
    end
    else begin
      portb_dout = ram[tick_portb_addr];
    end
  end

/*private:*/
  logic[15:0] ram[65536];
endmodule
