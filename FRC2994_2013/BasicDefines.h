
#ifndef BASICDEFINES_H_
#define BASICDEFINES_H_

// Types
typedef enum
{
	kEventErr, kEventNone, kEventOpened, kEventClosed
} EventType;
	
typedef enum
{
	kStateOpen, kStateClosed, kStateErr
} StateType;

typedef struct
{
	StateType state; 
	EventType event;
	bool enabled;
} ButtonEntry;

// Digital IOs

// Analog Inputs

// PWM Outputs

// Joystick 1

// Joystick 2
#define BUTTON_SHIFT 7

// Gamepad Buttons
#define BUTTON_FWD 4
#define BUTTON_REV 2
#define BUTTON_STOP 10

#endif
