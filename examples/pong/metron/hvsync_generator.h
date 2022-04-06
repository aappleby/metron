#include "metron_tools.h"

class HVSyncGenerator {
public:

	logic<1>  vga_h_sync;
	logic<1>  vga_v_sync;
	logic<1>  inDisplayArea;
	logic<10> CounterX;
	logic<9>  CounterY;

	void tock() {
		vga_h_sync = !vga_HS;
		vga_v_sync = !vga_VS;

		tick();
	}

private:

	logic<1> CounterXmaxed() const {
		return CounterX == b10(0x2FF);
	}

	void tick() {

		vga_HS = b6(CounterX,4) == b6(0x2D); // change this value to move the display horizontally
		vga_VS = CounterY == 500; // change this value to move the display vertically

		if(inDisplayArea==0)
			inDisplayArea = CounterXmaxed() && (CounterY < 480);
		else
			inDisplayArea = CounterX != 639;

		if (CounterXmaxed()) {
			CounterX = 0;
			CounterY = CounterY + 1;
		}
		else {
			CounterX = CounterX + 1;
		}
	}

	logic<1> vga_HS;
	logic<1> vga_VS;
};
