#include "WiiMoteWrapper.h"
#include <mmsystem.h>


using namespace std;



WiiMoteWrapper::WiiMoteWrapper()
{
	degrees = 0;
	buttonOne = buttonTwo = buttonHome = false;
	continueGame = true;
	status = 0; //0 = wachten 1 = succes -1 = niet succes
}

void WiiMoteWrapper::start(){
	wiiMoteMainStart();
}

WiiMoteWrapper::~WiiMoteWrapper()
{
	
}



// configs:
#define USE_BEEPS_AND_DELAYS			// undefine to test library works without them
#define LOOK_FOR_ADDITIONAL_WIIMOTES	// tries to connect any extra wiimotes

// ------------------------------------------------------------------------------------
//  state-change callback example (we use polling for everything else):
// ------------------------------------------------------------------------------------
void on_state_change(wiimote			  &remote,
	state_change_flags  changed,
	const wiimote_state &new_state)
{
	// we use this callback to set report types etc. to respond to key events
	//  (like the wiimote connecting or extensions (dis)connecting).

	// NOTE: don't access the public state from the 'remote' object here, as it will
	//		  be out-of-date (it's only updated via RefreshState() calls, and these
	//		  are reserved for the main application so it can be sure the values
	//		  stay consistent between calls).  Instead query 'new_state' only.

	// the wiimote just connected
	if (changed & CONNECTED)
	{
		// ask the wiimote to report everything (using the 'non-continous updates'
		//  default mode - updates will be frequent anyway due to the acceleration/IR
		//  values changing):

		// note1: you don't need to set a report type for Balance Boards - the
		//		   library does it automatically.

		// note2: for wiimotes, the report mode that includes the extension data
		//		   unfortunately only reports the 'BASIC' IR info (ie. no dot sizes),
		//		   so let's choose the best mode based on the extension status:
		remote.SetReportType(wiimote::IN_BUTTONS_ACCEL_IR_EXT); // no IR dots
	}
	// a MotionPlus was detected
	if (changed & MOTIONPLUS_DETECTED)
	{
		// enable it if there isn't a normal extension plugged into it
		// (MotionPlus devices don't report like normal extensions until
		//  enabled - and then, other extensions attached to it will no longer be
		//  reported (so disable the M+ when you want to access them again).
		if (remote.ExtensionType == wiimote_state::NONE) {
			bool res = remote.EnableMotionPlus();
			_ASSERT(res);
		}
	}
	// an extension is connected to the MotionPlus
	else if (changed & MOTIONPLUS_EXTENSION_CONNECTED)
	{
		// We can't read it if the MotionPlus is currently enabled, so disable it:
		if (remote.MotionPlusEnabled())
			remote.DisableMotionPlus();
	}
	// an extension disconnected from the MotionPlus
	else if (changed & MOTIONPLUS_EXTENSION_DISCONNECTED)
	{
		// enable the MotionPlus data again:
		if (remote.MotionPlusConnected())
			remote.EnableMotionPlus();
	}
	// another extension was just connected:
	else if (changed & EXTENSION_CONNECTED)
	{
#ifdef USE_BEEPS_AND_DELAYS
		//Beep(1000, 200);
#endif
		remote.SetReportType(wiimote::IN_BUTTONS_ACCEL_IR_EXT);
	}
	// extension was just disconnected:
	else if (changed & EXTENSION_DISCONNECTED)
	{
#ifdef USE_BEEPS_AND_DELAYS
		//Beep(200, 300);
#endif
	}
}
// ------------------------------------------------------------------------------------
void PrintTitle(HANDLE console)
{
	BRIGHT_WHITE;
	_tprintf(_T("\n"));
	_tprintf(_T("   -WiiYourself!- "));
	WHITE; _tprintf(_T("library Demo:   "));
	CYAN;  _tprintf(_T("| (c) "));
	BRIGHT_CYAN;  _tprintf(_T("gl"));
	BRIGHT_PURPLE; _tprintf(_T("."));
	BRIGHT_CYAN;  _tprintf(_T("tter"));
	CYAN; _tprintf(_T(" 2007-10\n")
		_T("                    v") WIIYOURSELF_VERSION_STR
		_T("         | http://gl.tter.org\n"));
	CYAN; _tprintf(_T(" ______________________________________________________________________\n\n\n"));
}
// ------------------------------------------------------------------------------------
int WiiMoteWrapper::wiiMoteMainStart()
{
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	// write the title
	//PrintTitle(console);
	// create a wiimote object
	wiimote remote;

	// in this demo we use a state-change callback to get notified of
	//  extension-related events, and polling for everything else
	// (note you don't have to use both, use whatever suits your app):
	remote.ChangedCallback = on_state_change;
	//  notify us only when the wiimote connected sucessfully, or something
	//   related to extensions changes
	remote.CallbackTriggerFlags = (state_change_flags)(CONNECTED | EXTENSION_CHANGED | MOTIONPLUS_CHANGED);
reconnect:
	COORD pos = { 0, 6 };
	//SetConsoleCursorPosition(console, pos);

	// try to connect the first available wiimote in the system
	//  (available means 'installed, and currently Bluetooth-connected'):
	//RED; _tprintf(_T("Looking for a Wiimote\n")); WHITE;

	//static const TCHAR* wait_str[] = { _T(".  "), _T(".. "), _T("...") };
	unsigned count = 0;
	while (!remote.Connect(wiimote::FIRST_AVAILABLE) && continueGame) {
		//_tprintf(_T("\b\b\b\b%s "), wait_str[count % 3]);
		count++;
#ifdef USE_BEEPS_AND_DELAYS
		//Beep(500, 30); 
		Sleep(1000);
#endif
	}
	if (!continueGame){
		return 0;
	}

	// connected - light all LEDs
	remote.SetLEDs(0x0f);
	BRIGHT_CYAN; _tprintf(_T("\b\b\b\b... connected!")); WHITE;
	status = 1;
#ifdef USE_BEEPS_AND_DELAYS
	//Beep(1000, 300); 
	Sleep(2000);
#endif
	COORD cursor_pos = { 0, 6 };
	// print the button event instructions:
	//BRIGHT_WHITE;	_tprintf(_T("\r  -- TRY: B = rumble, A = square, 1 = sine, 2 = daisy, Home = Exit --\n"));
	while (continueGame)
	{
		// IMPORTANT: the wiimote state needs to be refreshed each pass
		while (remote.RefreshState() == NO_CHANGE)
			Sleep(1); // // don't hog the CPU if nothing changed
		cursor_pos.Y = 8;
		//SetConsoleCursorPosition(console, cursor_pos);
		if (remote.ConnectionLost())
		{
			//BRIGHT_RED; _tprintf(			_T("   *** connection lost! ***                                          \n\n\n\n\n\n\n\n\n\n\n"));
			//Beep(100, 1000);
			Sleep(2000);
			COORD pos = { 0, 6 };
			//SetConsoleCursorPosition(console, pos);
			//printf("\n\n\n");
			goto reconnect;
		}

		// rumble if 'B' (trigger) is pressed
		//remote.SetRumble(remote.Button.B());

		// TEMP: Minus button disables MotionPlus (if connected) to allow its
		//        own extension port to work
		static bool last_minus = false;
		if (remote.Button.Minus() && !last_minus &&
			(remote.ExtensionType == wiimote_state::MOTION_PLUS))
			remote.DisableMotionPlus();
		last_minus = remote.Button.Minus();

		// actions for buttons just pressed/released:
		static bool last_A = false, last_One = false, last_Two = false;

#define ON_PRESS_RELEASE(button, pressed_action, released_action)	\
			{ bool pressed = remote.Button.button();						\
			  if(pressed)													\
			  			  			  			 	{				   /* just pressed? */						\
				if(!last_##button) pressed_action;							\
			  			  			  				}															\
									else if(last_##button) /* just released */						\
				released_action;											\
			/* remember the current button state for next time */			\
			last_##button = pressed; }

		//  play audio whilst certain buttons are held
		//ON_PRESS_RELEASE(A, remote.PlaySquareWave(FREQ_3130HZ, 0x20),remote.EnableSpeaker(false));
		// Battery level:
		/*CYAN; _tprintf(_T("  Battery: "));
		(remote.bBatteryDrained) ? BRIGHT_RED :
			(remote.BatteryPercent >= 30) ? BRIGHT_GREEN : BRIGHT_YELLOW;
		_tprintf(_T("%3u%%   "), remote.BatteryPercent);*/
		// Rumble
		//WHITE; _tprintf(_T("] "));
		//if (remote.bRumble) {BRIGHT_WHITE; printf("RUMBLE ");}	else_tprintf(_T("       "));
		// Output method:
		//CYAN; _tprintf(_T("        using %s\n"), (remote.IsUsingHIDwrites() ?
//			_T("HID writes") : _T("WriteFile()")));

		// Buttons:
		//CYAN; _tprintf(_T("  Buttons: ")); WHITE; _tprintf(_T("["));
		for (unsigned bit = 0; bit < 16; bit++)
		{
			WORD mask = (WORD)(1 << bit);
			// skip unused bits
			if ((wiimote_state::buttons::ALL & mask) == 0)
				continue;

			const TCHAR* button_name = wiimote::ButtonNameFromBit[bit];
			bool		 pressed = ((remote.Button.Bits & mask) != 0);
			//if (bit > 0) {	CYAN; _tprintf(_T("|")); // seperator	}
			if (button_name == _T("One"))
			{
				buttonOne = pressed;
			}
			if (button_name == _T("Two"))
			{
				buttonTwo = pressed;
			}
			if (button_name == _T("Home"))
			{
				buttonHome = pressed;
			}
			//if (pressed) {
			//	BRIGHT_WHITE; _tprintf(_T("%s"), button_name);
			//}
			//else{
			//	WHITE; _tprintf(_T("%*s"), _tcslen(button_name), _T(""));
			//}
		}
		//WHITE; _tprintf(_T("]\n"));

		// Acceleration:
		//CYAN; _tprintf(_T("    Accel:")); WHITE;
		//_tprintf(_T("  X %+2.3f  Y %+2.3f  Z %+2.3f  \n"),remote.Acceleration.X,remote.Acceleration.Y,remote.Acceleration.Z);

		// Orientation estimate (shown red if last valid update is aging):
		//CYAN; _tprintf(_T("   Orient:")); WHITE;		_tprintf(_T("  UpdateAge %3u  "), remote.Acceleration.Orientation.UpdateAge);

		//  show if the last orientation update is considered out-of-date
		//   (using an arbitrary threshold)
		//if (remote.Acceleration.Orientation.UpdateAge > 10)
		//	RED;
		degrees = remote.Acceleration.Orientation.Pitch;
		remote.MotionPlus.Speed.Yaw;
		while (degrees > 360.0) degrees -= 360.0;
		while (degrees < -360.0) degrees += 360.0;
		/*_tprintf(_T("Pitch:%4ddeg  Roll:%4ddeg  \n")
			_T("                           (X %+.3f  Y %+.3f  Z %+.3f)      \n"),
			(int)remote.Acceleration.Orientation.Pitch,
			(int)remote.Acceleration.Orientation.Roll,
			remote.Acceleration.Orientation.X,
			remote.Acceleration.Orientation.Y,
			remote.Acceleration.Orientation.Z);*/
		// Speaker:
		/*CYAN; _tprintf(_T("  Speaker:"));
		WHITE;
		_tprintf(_T("  %s | %s    "), (remote.Speaker.bEnabled ? _T("On ") :
			_T("Off")),
			(remote.Speaker.bMuted ? _T("Muted") :
			_T("     ")));
		_tprintf(_T("Frequency %4u Hz   Volume 0x%02x\n"),
			wiimote::FreqLookup[remote.Speaker.Freq],
			remote.Speaker.Volume);*/
	}
	// disconnect (auto-happens on wiimote destruction anyway, but let's play nice)
	remote.Disconnect();
	CloseHandle(console);
	return 0;
}