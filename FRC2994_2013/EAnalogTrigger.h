#ifndef EANALOGTRIGGER_H_
#define EANALOGTRIGGER_H_

#define EANALOGTRIGGER
#include "WPILib.h"
#include "AnalogTrigger.h"
#include "BasicDefines.h"



class EAnalogTrigger : public AnalogTrigger
{
public:
	EAnalogTrigger(UINT32 channel);
	EAnalogTrigger(UINT8 moduleNumber, UINT32 channel);
	EAnalogTrigger(AnalogChannel *channel);
	~EAnalogTrigger();
	
	EventType GetEvent();
	StateType GetState();
	void Update();
	
private: 
	void Initialize();
	
	ButtonEntry m_trigger;
};
#endif
