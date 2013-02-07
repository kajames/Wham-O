#ifndef EJOYSTICK_H_
#define EJOYSTICK_H_

#define EJOYSTICK_NUM_BUTTONS 11
#include "WPILib.h"
#include "BasicDefines.h"

class EJoystick : public Joystick
{
public:
	explicit EJoystick(UINT32 port);
	EJoystick(UINT32 port, UINT32 numAxisTypes, UINT32 numButtonTypes);
	virtual ~EJoystick();

	EventType GetEvent(int buttonNumber);
	StateType GetState(int buttonNumber);
	void Update();
	bool EnableButton(int buttonNumber);
	
	
private: 
	void Initialize();
	
	ButtonEntry m_buttonTable[EJOYSTICK_NUM_BUTTONS+1];
};
#endif
