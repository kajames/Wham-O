#include "EGamepad.h"


EGamepad::EGamepad(UINT32 port):
	Gamepad(port)
{
	Initialize();
};

EGamepad::~EGamepad()
{
};

EventType EGamepad::GetEvent(int buttonNumber)
{
	if (buttonNumber < 1 || buttonNumber > EGAMEPAD_NUM_BUTTONS)
	{
		return kEventErr;
	}
	
	//Disabled buttons are initialized to kEventErr
	return m_buttonTable[buttonNumber].event;
}

StateType EGamepad::GetState(int buttonNumber)
{
	if (buttonNumber < 1 || buttonNumber > EGAMEPAD_NUM_BUTTONS)
	{
		return kStateErr;
	}
	
	//Disabled buttons are initialized to kStateErr
	return m_buttonTable[buttonNumber].state;
}

void EGamepad::Update()
{
	StateType newState;
	
	for (int i=1; i<=EGAMEPAD_NUM_BUTTONS; i++)
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


bool EGamepad::EnableButton(int buttonNumber)
{
	if (buttonNumber < 1 || buttonNumber > EGAMEPAD_NUM_BUTTONS)
	{
		return false;
	}
	
	m_buttonTable[buttonNumber].enabled = true;
	return true;
}

void EGamepad::Initialize()
{
	m_buttonTable[0].enabled = false;
	m_buttonTable[0].event = kEventErr;
	m_buttonTable[0].state = kStateErr;
	
	for (int i=1; i<=EGAMEPAD_NUM_BUTTONS; i++)
	{
		m_buttonTable[i].enabled = false;
		m_buttonTable[i].event = kEventErr;
		m_buttonTable[i].state = GetRawButton(i) ? kStateClosed : kStateOpen;
	}
}
