`include "metron_tools.sv"

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

	always_comb begin /*tock*/
		vga_h_sync = !vga_HS;
		vga_v_sync = !vga_VS;

		/*tick()*/;
	end

/*private:*/

	function logic CounterXmaxed() /*const*/; 
		CounterXmaxed = CounterX == 10'h2FF;
	endfunction

	task tick(); 

		vga_HS <= CounterX[9:4] == 6'h2D; // change this value to move the display horizontally
		vga_VS <= CounterY == 500; // change this value to move the display vertically

		if(inDisplayArea==0)
			inDisplayArea <= CounterXmaxed() && (CounterY < 480);
		else
			inDisplayArea <= CounterX != 639;

		if (CounterXmaxed()) begin
			CounterX <= 0;
			CounterY <= CounterY + 1;
		end
		else begin
			CounterX <= CounterX + 1;
		end
	endtask
	always_ff @(posedge clock) tick();

	logic vga_HS;
	logic vga_VS;
endmodule

