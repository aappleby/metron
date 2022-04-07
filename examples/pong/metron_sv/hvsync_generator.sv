`ifndef HVSYNC_GENERATOR_H
`define HVSYNC_GENERATOR_H

`include "metron_tools.sv"

//------------------------------------------------------------------------------

module HVSyncGenerator
(
	input logic clock,
	output logic  vga_h_sync,
	output logic  vga_v_sync,
	output logic  inDisplayArea,
	output logic[9:0] CounterX,
	output logic[8:0]  CounterY
);
/*public:*/

	/*logic<1>  vga_h_sync;*/
	/*logic<1>  vga_v_sync;*/
	/*logic<1>  inDisplayArea;*/
	/*logic<10> CounterX;*/
	/*logic<9>  CounterY;*/

	task tick_counters(); 
		logic[9:0] next_x;
		logic[9:0] next_y;
		next_x = CounterX == 767 ? 0 : CounterX + 1;
		next_y = next_x == 0 ? 10'(CounterY + 1) : 10'(CounterY);

		vga_h_sync <= !((next_x >= 720) && (735 >= next_y));
		vga_v_sync <= !(next_y == 500);

		inDisplayArea <= (next_x < 640) && (next_y < 480);

		CounterX <= next_x;
		CounterY <= next_y;
	endtask
	always_ff @(posedge clock) tick_counters();
endmodule;

//------------------------------------------------------------------------------

`endif // HVSYNC_GENERATOR_H

