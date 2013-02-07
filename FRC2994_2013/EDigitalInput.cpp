#include "EDigitalInput.h"

EDigitalInput::EDigitalInput(UINT32 channel):
	DigitalInput(channel)
{
	Initialize();
};

EDigitalInput::EDigitalInput(UINT8 moduleNumber, UINT32 channel):
	DigitalInput(moduleNumber, channel)
{
	Initialize();
};

EDigitalInput::~EDigitalInput()
{
};

EventType EDigitalInput::GetEvent()
{
	
	//Disabled buttons are initialized to kEventErr
	return m_button.event;
}

StateType EDigitalInput::GetState()
{
	return m_button.state;
}

void EDigitalInput::Update()
{
	StateType newState;
	
			newState = Get() ? kStateOpen: kStateClosed;
			if (newState == m_button.state)
			{
				m_button.event = kEventNone;
			}
			else
			{
				if (kStateClosed == newState)
				{
					m_button.event = kEventClosed;
				}
				else
				{
					m_button.event = kEventOpened;
				}
				m_button.state = newState;
			}
}


void EDigitalInput::Initialize()
{
	m_button.enabled = false;
	m_button.event = kEventErr;
	m_button.state = kStateErr;
	
}
