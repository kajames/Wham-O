#ifndef EGAMEPAD_H_
#define EGAMEPAD_H_

#define EGAMEPAD_NUM_BUTTONS 12
#include "WPILib.h"
#include "Gamepad.h"
#include "BasicDefines.h"

// To use, call the initialize method once before you enter your main program
// loop then call the update method once per loop


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
