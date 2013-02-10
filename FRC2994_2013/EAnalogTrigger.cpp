#include "EAnalogTrigger.h"

EAnalogTrigger::EAnalogTrigger(UINT32 channel):
	AnalogTrigger(channel)
{
	Initialize();
};

EAnalogTrigger::EAnalogTrigger(UINT8 moduleNumber, UINT32 channel):
	AnalogTrigger(moduleNumber, channel)
{
	Initialize();
};

EAnalogTrigger::EAnalogTrigger(AnalogChannel *channel):
		AnalogTrigger(channel)
{
	Initialize();
}

EAnalogTrigger::~EAnalogTrigger()
{
}

EventType EAnalogTrigger::GetEvent()
{
	
	return m_trigger.event;
}

StateType EAnalogTrigger::GetState()
{
	
	return m_trigger.state;
}

void EAnalogTrigger::Update()
{
	StateType newState;
	
	newState = GetTriggerState() ? kStateOpen : kStateClosed;
	if (newState == m_trigger.state)
	{
		m_trigger.event = kEventNone;
	}
	else
	{
		if (kStateClosed == newState)
		{
			m_trigger.event = kEventClosed;
		}
		else
		{
			m_trigger.event = kEventOpened;
		}
		m_trigger.state = newState;
	}
}

void EAnalogTrigger::Initialize()
{
	m_trigger.enabled = false;
	m_trigger.event = kEventErr;
	m_trigger.state = kStateErr;
	SetLimitsVoltage(1.0, 4.0);
	SetAveraged(false);
	SetFiltered(false);
	
}
