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
	
	// Disabled buttons are initialized to kEventErr (just sayin')
	return m_buttonTable[buttonNumber].event;
}

StateType EGamepad::GetState(int buttonNumber)
{
	if (buttonNumber < 1 || buttonNumber > EGAMEPAD_NUM_BUTTONS)
	{
		return kStateErr;
	}
	
	// Disabled buttons are initialized to kStateErr (just sayin' again)
	return m_buttonTable[buttonNumber].state;
}

EventType EGamepad::GetDPadEvent(Gamepad::DPadDirection direction)
{
	return m_dpadTable[direction].event;
}

StateType EGamepad::GetDPadState(Gamepad::DPadDirection direction)
{
	return m_dpadTable[direction].state;
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
	
	Gamepad::DPadDirection dpadDirection = GetDPad();
	  
	// By definition, only one contact on the dpad, at most, can be pressed at
	// any one time. So, we loop through the table using a newState of "open"
	// for every entry except the one (if any) that is being pressed. Note that
	// a kCenter direction is equivalent to nothing pressed.
	
	for (int i=Gamepad::kCenter; i<=Gamepad::kUpRight; i++)
	{
		if (i == dpadDirection)
		{
			newState = kStateClosed;
		}
		else
		{
			newState = kStateOpen;
		}
		
		if (newState == m_dpadTable[i].state)
		{
			m_dpadTable[i].event = kEventNone;
		}
		else
		{
			if (kStateClosed == newState)
			{
				m_dpadTable[i].event = kEventClosed;
			}
			else
			{
				m_dpadTable[i].event = kEventOpened;
			}
			m_dpadTable[i].state = newState;
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
	
	// Only one DPad direction can be closed at any one time (kCenter is returned 
	// when no direction is closed). We initialize all the directions to open then
	// read the DPad and set closed the direction read (which will be, unless someone
	// is messing with the DPad at init time, kCentre)
	
	for (int i=Gamepad::kCenter; i<=Gamepad::kUpRight; i++)
	{
		m_dpadTable[i].enabled = false; // We never check this
		m_dpadTable[i].event = kEventNone;
		m_dpadTable[i].state = kStateOpen;
	}
	
	m_dpadTable[GetDPad()].state = kStateClosed;
}
