
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

typedef struct
{
	float speed_left;
	float speed_right;
	float distance;
} step_speed;

// Shaft Encoder distance/pulse
// 8 inch wheel: PI*8/360 = 3.14159265*8/360 = .06981317 inches per pulse
#define DRIVE_ENCODER_DISTANCE_PER_PULSE 	0.06981317

// Table over motor speeds and travel distances for autonomous mode
#define NUM_AUTONOMOUS_STEPS 2

// Each entry in this table describes one move step in autonomous mode. The first
// two entries in each row are the left and right motor speeds, respectively. The 
// last entry is the absolute distance (in inches) that the left wheel must turn
// before the step is complete.
const step_speed m_autoForward[NUM_AUTONOMOUS_STEPS] =
{
		{0.4,-0.4,500},
		{-0.4, 0.4, 300}
};

// Maximum Shifts Allowed (to make sure there is enough air for claw locks when 
// climbing
#define MAX_SHIFTS 50

// Motor controller settings
#define COLLECTOR_FWD 1.0
#define COLLECTOR_REV -1.0
#define INDEXER_FWD 0.6
#define SHOOTER_FWD 1.0
#define ARM_DESCEND 1.0
#define ARM_CLIMB -1.0
#define CLIMB_LIMIT .5f
#define DESCEND_LIMIT 4.5f

// Define the target platfomr we are building for. We need this
// because the hardware on Plyboy, WhamO-1 and WhamO-2 are 
// all different from one another
//#define PLYBOY
//#define WHAMO1
#define WHAMO2

#ifdef PLYBOY
#define NAME "Plyboy"
#define LEFT_FRONT_DRIVE_MOTOR Jaguar
#define RIGHT_DRIVE_MOTOR Jaguar
#define COLLECTOR_MOTOR_CONTROLLER Jaguar
#define INDEXER_MOTOR_CONTROLLER Jaguar
#define SHOOTER_MOTOR_CONTROLLER Jaguar
#define ARM_MOTOR_CONTROLLER Jaguar
#define GREEN_CLAW_LOCK_STATE (greenClawLockSwitch.GetState())
#define YELLOW_CLAW_LOCK_STATE (yellowClawLockSwitch.GetState())
#endif

#ifdef WHAMO1
#define NAME "WhamO-1"
#define LEFT_DRIVE_MOTOR Jaguar		// This is very strange, but seems to work...
#define RIGHT_DRIVE_MOTOR Jaguar	// TODO: Should be Victor. Victor causes no response.
#define COLLECTOR_MOTOR_CONTROLLER Jaguar
#define INDEXER_MOTOR_CONTROLLER Jaguar
#define SHOOTER_MOTOR_CONTROLLER Jaguar
#define ARM_MOTOR_CONTROLLER Jaguar
#define GREEN_CLAW_LOCK_STATE (!greenClawLockSwitch.GetState())
#define YELLOW_CLAW_LOCK_STATE (!yellowClawLockSwitch.GetState())
#endif

#ifdef WHAMO2
#define NAME "WhamO-2"
#define LEFT_DRIVE_MOTOR Jaguar
#define RIGHT_DRIVE_MOTOR Jaguar
#define COLLECTOR_MOTOR_CONTROLLER Jaguar
#define INDEXER_MOTOR_CONTROLLER Jaguar
#define SHOOTER_MOTOR_CONTROLLER Jaguar
#define ARM_MOTOR_CONTROLLER Jaguar
#define GREEN_CLAW_LOCK_STATE (!greenClawLockSwitch.GetState())
#define YELLOW_CLAW_LOCK_STATE (!yellowClawLockSwitch.GetState())
#endif

// Digital IOs
#define LEFT_DRIVE_ENC_A 1
#define LEFT_DRIVE_ENC_B 2
#define RIGHT_DRIVE_ENC_A 3
#define RIGHT_DRIVE_ENC_B 4
#define CLAW_1_LOCK_SENSOR 5
#define CLAW_2_LOCK_SENSOR 6
#define INDEXER_SW 7
#define COMPRESSOR_PRESSURE_SW 8
#define UNUSED_DIO_9 9
#define UNUSED_DIO_10 10
#define UNUSED_DIO_11 11
#define UNUSED_DIO_12 12
#define UNUSED_DIO_13 13
#define UNUSED_DIO_14 14

// Analog Inputs
#define UNUSED_ANALOG_INPUT_1 1
#define UNUSED_ANALOG_INPUT_2 2
#define UNUSED_ANALOG_INPUT_3 3
#define UNUSED_ANALOG_INPUT_4 4
#define ARM_ROTATION_POT 5
#define UNUSED_ANALOG_INPUT_6 6
#define UNUSED_ANALOG_INPUT_7 7

// PWM Outputs
#define LEFT_FRONT_DRIVE_PWM 1
#define LEFT_REAR_DRIVE_PWM 2
#define RIGHT_FRONT_DRIVE_PWM 3
#define RIGHT_REAR_DRIVE_PWM 4
#define ARM_PWM 5
#define UNUSED_PWM_OUTPUT_6 6
#define SHOOTER_PWM 7
#define PICKUP_PWM 8
#define UNUSED_PWM_OUTPUT_9 9
#define INDEX_PWM 10

// Joystick 1
#define BUTTON_RUN_SHOOTER 1
#define UNUSED_BUTTON_2 2
#define UNUSED_BUTTON_3 3
#define BUTTON_INDEXER 4 
#define UNUSED_BUTTON_5 5
#define UNUSED_BUTTON_6 6
#define UNUSED_BUTTON_7 7
#define UNUSED_BUTTON_8 8
#define UNUSED_BUTTON_9 9
#define UNUSED_BUTTON_10 10
#define UNUSED_BUTTON_11 11

// Joystick 2
#define BUTTON_RUN_SHOOTER 1
#define UNUSED_BUTTON_2 2
#define UNUSED_BUTTON_3 3
#define BUTTON_INDEXER 4 
#define UNUSED_BUTTON_5 5
#define UNUSED_BUTTON_6 6
#define BUTTON_SHIFT 7
#define UNUSED_BUTTON_8 8
#define UNUSED_BUTTON_9 9
#define UNUSED_BUTTON_10 10
#define UNUSED_BUTTON_11 11


// Gamepad Buttons
#define BUTTON_RUN_SHOOTER 1
#define BUTTON_COLLECTOR_REV 2
#define BUTTON_SHOOTER 3
#define BUTTON_COLLECTOR_FWD 4
#define BUTTON_CLAW_1_LOCKED 5
#define BUTTON_CLAW_2_LOCKED 6
#define BUTTON_CLAW_1_UNLOCKED 7
#define BUTTON_CLAW_2_UNLOCKED 8
#define UNUSED_BUTTON_9 9
#define BUTTON_STOP_ALL 10
#define BUTTON_JOG_FWD 11
#define BUTTON_JOG_REV 12


//Relays
#define INDEXER_SPIKE 1
#define COMPRESSOR_SPIKE 2
#define UNUSED_RELAY_3 4
#define UNUSED_RELAY_4 4
#define UNUSED_RELAY_5 5
#define UNUSED_RELAY_6 6
#define UNUSED_RELAY_7 7
#define UNUSED_RELAY_8 8

//Solenoid
#define SHIFTER_A 1
#define SHIFTER_B 2
#define CLAW_1_LOCKED 3
#define CLAW_1_UNLOCKED 4
#define CLAW_2_LOCKED 5
#define CLAW_2_UNLOCKED 6
#define UNUSED_SOLENOID_7 7
#define UNUSED_SOLENOID_8 8

// Various Constants
#define JOG_TIME 0.2
#define SPINUP_TIME 3.0

#endif
