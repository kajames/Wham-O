#include "EJoystick.h"


EJoystick::EJoystick(UINT32 port):
	Joystick(port)
{
	Initialize();
};

EJoystick::~EJoystick()
{
};

EJoystick::EJoystick(UINT32 port, UINT32 numAxisTypes, UINT32 numButtonTypes):
		Joystick(port, numAxisTypes, numButtonTypes)
{
	Initialize();
};

EventType EJoystick::GetEvent(int buttonNumber)
{
	if (buttonNumber < 1 || buttonNumber > EJOYSTICK_NUM_BUTTONS)
	{
		return kEventErr;
	}
	
	//Disabled buttons are initialized to kEventErr
	return m_buttonTable[buttonNumber].event;
}

StateType EJoystick::GetState(int buttonNumber)
{
	if (buttonNumber < 1 || buttonNumber > EJOYSTICK_NUM_BUTTONS)
	{
		return kStateErr;
	}
	
	//Disabled buttons are initialized to kStateErr
	return m_buttonTable[buttonNumber].state;
}

void EJoystick::Update()
{
	StateType newState;
	
	for (int i=1; i<=EJOYSTICK_NUM_BUTTONS; i++)
	{
		if (true == m_buttonTable[i].enabled)
		{
			newState = GetRawButton(i) ? kStateClosed : kStateOpen;
			if (newState == m_buttonTable[i].state)
			{
				m_buttonTable[i].event = kEventNone;
			}
			else
			{
				if (kStateClosed == newState)
				{
					m_buttonTable[i].event = kEventClosed;
				}
				else
				{
					m_buttonTable[i].event = kEventOpened;
				}
				m_buttonTable[i].state = newState;
			}
		}
	}
}


bool EJoystick::EnableButton(int buttonNumber)
{
	if (buttonNumber < 1 || buttonNumber > EJOYSTICK_NUM_BUTTONS)
	{
		return false;
	}
	
	m_buttonTable[buttonNumber].enabled = true;
	return true;
}

void EJoystick::Initialize()
{
	m_buttonTable[0].enabled = false;
	m_buttonTable[0].event = kEventErr;
	m_buttonTable[0].state = kStateErr;
	
	for (int i=1; i<=EJOYSTICK_NUM_BUTTONS; i++)
	{
		m_buttonTable[i].enabled = false;
		m_buttonTable[i].event = kEventErr;
		m_buttonTable[i].state = GetRawButton(i) ? kStateClosed : kStateOpen;
	}
}
