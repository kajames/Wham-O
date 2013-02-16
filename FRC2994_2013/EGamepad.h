#ifndef EGAMEPAD_H_
#define EGAMEPAD_H_

#include "WPILib.h"
#include "Gamepad.h"
#include "BasicDefines.h"

#define EGAMEPAD_NUM_BUTTONS 12
// This class extends the Gamepad class to provide button event detection

// To use, call EnableButton once to enable tracking of the button
// you want, call the Update method once before you enter your main program
// loop, and then call the Update method once per loop. At any point in
// your program loop you can check the state of the button using GetState
// and check for a state change (an "event" closed->open or open->closd)
// using GetEvent.

class EGamepad : public Gamepad
{
public:
	explicit EGamepad(UINT32 port);
	~EGamepad();

	EventType GetEvent(int buttonNumber);
	StateType GetState(int buttonNumber);
	EventType EGamepad::GetDPadEvent(Gamepad::DPadDirection direction);
	StateType EGamepad::GetDPadState(Gamepad::DPadDirection direction);

	void Update();
	bool EnableButton(int buttonNumber);
	
private: 
	void Initialize();
	
	ButtonEntry m_buttonTable[EGAMEPAD_NUM_BUTTONS+1];
	ButtonEntry m_dpadTable[Gamepad::kUpRight+1];
};
#endif
