#ifndef EDIGITALINPUT_H_
#define EDIGITALINPUT_H_

#define EDIGITALINPUT_NUM_BUTTONS 12
#include "WPILib.h"
#include "DigitalInput.h"
#include "BasicDefines.h"

// To use, call the initialize method once before you enter your main program
// loop then call the update method once per loop


class EDigitalInput : public DigitalInput
{
public:
	 EDigitalInput(UINT32 channel);
	 EDigitalInput(UINT8 moduleNumber, UINT32 channel);
	~EDigitalInput();

	EventType GetEvent();
	StateType GetState();
	void Update();
	
private: 
	void Initialize();
	
	ButtonEntry m_button;
};
#endif
