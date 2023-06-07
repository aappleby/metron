#include <stdio.h>

/*
The output signals are:
	MAIN DISPLAY (MAIN DISPLAY TYPE) : towards the main display
	MINI DISPLAY (MINI DISPLAY TYPE) : towards the mini display
	ALPHABETIC DISPLAY (string) : towards the alphabetic display
	START ENHANCING (DISPLAY POSITION) : for enhancing a display position
	STOP ENHANCING (DISPLAY POSITION) : for stopping enhancing a display position
	CHIME STATUS (boolean) : towards the chime status indicator
	STOPWATCH RUN STATUS (boolean) : towards the stopwatch run status indicator
	STOPWATCH LAP STATUS (boolean) : towards the stopwatch lap status indicator
	ALARM STATUS (boolean) : towards the alarm status indicator
	BEEP (combine BEEP TYPE with COMBINE BEEPS) : towards the beeper
*/

/*
6.1. The WATCH module interface
This module handles both the watch and set-watch modes. The types involved are WATCH TIME TYPE,
WATCH TIME POSITION, and BEEP TYPE.
All input signals are assumed to be pairwise incompatible.

The input signals are:
	S : the second
	TOGGLE 24H MODE COMMAND : go from 24H mode to 12H and conversely
	TOGGLE CHIME COMMAND : toggle the chime from on to o and conversely
	ENTER SET WATCH MODE COMMAND : start a setting sequence
	SET WATCH COMMAND : apply a setting command
	NEXT WATCH TIME POSITION COMMAND : go to the next setting position
	EXIT SET WATCH MODE COMMAND : terminate the setting sequence
The output signals are:
	WATCH TIME (WATCH TIME TYPE): the current time
	WATCH BEING SET : a pure signal, always synchronous with WATCH TIME, which tells that the watch is currently in a setting sequence.
	START ENHANCING (WATCH TIME POSITION): emitted in setting sequences when a position becomes the currently set position
	STOP ENHANCING (WATCH TIME POSITION): emitted in setting sequences when a position stops being the currently set position or when set-watch mode is exited
	CHIME STATUS (boolean): towards the chime status indicator
	BEEP (BEEP TYPE): towards the beeper
*/

/*
enum WATCH_MODE {
	MODE_WATCH,
	MODE_SET,
	MODE_STOPWATCH
};

enum CURSOR {
	CURSOR_SECOND = 0,
	CURSOR_HOUR,
	CURSOR_10MIN,
	CURSOR_MINUTE,
	CURSOR_MONTH,
	CURSOR_DAY,
	CURSOR_DAY_OF_WEEK,
	CURSOR_MAX
};

struct Wristwatch {

	//----------------------------------------
	// Stopwatch

	int STOPWATCH_TIME;
	int STOPWATCH_RUN_STATUS;
	int STOPWATCH_LAP_STATUS;
	int STOPWATCH_BEEP;

	void tick_stopwatch(int HS, int START_STOP_COMMAND, int LAP_COMMAND) {
	}

	//----------------------------------------
	// Alarm

	int ALARM_TIME;
	int ALARM_START_ENHANCING;
	int ALARM_STOP_ENHANCING;
	int ALARM_STATUS;
	int ALARM_BEEP;

	void tick_alarm(
		int TOGGLE_24H_MODE_COMMAND,
		int ENTER_SET_ALARM_MODE_COMMAND,
		int SET_ALARM_COMMAND,
		int NEXT_ALARM_TIME_POSITION_COMMAND,
		int EXIT_SET_ALARM_MODE_COMMAND,
		int WATCH_TIME,
		int WATCH_BEING_SET,
		int TOGGLE_ALARM_COMMAND,
		int S,
		int STOP_ALARM_BEEP_COMMAND) {
	}

	//----------------------------------------
	// Buttons

	int WATCH_MODE_COMMAND;
	int TOGGLE_24H_MODE_COMMAND;
	int ENTER_SET_WATCH_MODE_COMMAND;
	int SET_WATCH_COMMAND;
	int NEXT_WATCH_TIME_POSITION_COMMAND;
	int EXIT_SET_WATCH_MODE_COMMAND;
	int TOGGLE_CHIME_COMMAND;
	int STOPWATCH_MODE_COMMAND;
	int START_STOP_COMMAND;
	int LAP_COMMAND;
	int ALARM_MODE_COMMAND;
	int ENTER_SET_ALARM_MODE_COMMAND;
	int SET_ALARM_COMMAND;
	int NEXT_ALARM_TIME_POSITION_COMMAND;
	int EXIT_SET_ALARM_MODE_COMMAND;
	int TOGGLE_ALARM_COMMAND;
	int STOP_ALARM_BEEP_COMMAND;

	void tick_buttons(int UL, int UR, int LL, int LR) {
	}

	//----------------------------------------
	// Top

	void tick_mode_watch(int UL, int UR, int LL, int LR, int TIME_S, int TIME_HS) {
		if (UL) {
			mode@ = MODE_SET;
		}
		if (UR) {
		}
		if (LL) {
			mode@ = MODE_STOPWATCH;
		}
		if (LR) {
			mode_24h = !mode_24h;
		}
	}

	void tick_mode_stopwatch(int UL, int UR, int LL, int LR, int TIME_S, int TIME_HS) {
		if (UL) {
		}
		if (UR) {
			// lap
		}
		if (LL) {
			mode@ = MODE_WATCH;
		}
		if (LR) {
			// start/stop
		}
	}

	void tick_mode_set(int UL, int UR, int LL, int LR, int TIME_S, int TIME_HS) {
		if (UL) {
			mode@ = MODE_WATCH;
		}
		if (UR) {
		}
		if (LL) {
			cursor = (cursor + 1) % CURSOR_MAX;
		}
		if (LR) {
			// apply setting command
		}
	}

	void tick(int UL, int UR, int LL, int LR, int TIME_S, int TIME_HS) {
		switch (mode) {
			case MODE_WATCH:     tick_mode_watch(UL, UR, LL, LR, TIME_S, TIME_HS); break;
			case MODE_STOPWATCH: tick_mode_stopwatch(UL, UR, LL, LR, TIME_S, TIME_HS); break;
			case MODE_SET:       tick_mode_set(UL, UR, LL, LR, TIME_S, TIME_HS); break;
		}
	}

	//----------------------------------------

	int mode = MODE_WATCH;
	int mode_24h = 1;
};


int main(int argc, char** argv) {
	printf("Hello World\n");
	return 0;
}
*/
