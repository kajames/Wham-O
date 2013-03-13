#include "WPILib.h"
#include "BasicDefines.h"
#include "EGamepad.h"
#include "EAnalogTrigger.h"
#include "EDigitalInput.h"
#include "Ejoystick.h"
#include "NetworkTables/NetworkTable.h"
#include "Timer.h"

/**
 * This is Wham-O - the 2013 2994 Robot
 */ 

class RobotDemo : public SimpleRobot
{
	// Drive motors
	LEFT_DRIVE_MOTOR leftFrontDriveMotor; // Done BEFORE the robot drive which uses them
	LEFT_DRIVE_MOTOR leftRearDriveMotor;
	RIGHT_DRIVE_MOTOR rightFrontDriveMotor;
	RIGHT_DRIVE_MOTOR rightRearDriveMotor;
	
	// Drive system
	RobotDrive myRobot; // robot drive system
	
	// Input Devices
	Joystick  stick; 
	EJoystick stick2;
	EGamepad  gamepad;
	
	// Other Motor Controllers (macros used because of differing hw on plyboy and bots)
	INDEXER_MOTOR_CONTROLLER   indexerMotor;
	SHOOTER_MOTOR_CONTROLLER   shooterMotor;
	ARM_MOTOR_CONTROLLER       armMotor;
	
	// Shaft encoders
	Encoder leftDriveEncoder;
	Encoder rightDriveEncoder;
	
	// Output Devices
	DoubleSolenoid shifter;
	DoubleSolenoid greenClaw;
	DoubleSolenoid yellowClaw;
	
	// Input sensors
	AnalogChannel potentiometer;
	EDigitalInput indexerSwitch;
	EDigitalInput greenClawLockSwitch;
	EDigitalInput yellowClawLockSwitch;
	
	// Miscellaneous
	Compressor compressor;
	Timer jogTimer;
	Timer shooterTimer;

	// Nonobject members
	bool m_shooterMotorRunning;
	bool m_jogTimerRunning;
	int  m_shiftCount;
	float m_previousPot;
	
	DriverStationLCD *dsLCD;

public:
	RobotDemo(void):
		leftFrontDriveMotor(LEFT_FRONT_DRIVE_PWM),
		leftRearDriveMotor(LEFT_REAR_DRIVE_PWM),
		rightFrontDriveMotor(RIGHT_FRONT_DRIVE_PWM),
		rightRearDriveMotor(RIGHT_REAR_DRIVE_PWM),
		myRobot(&leftFrontDriveMotor, &leftRearDriveMotor, &rightFrontDriveMotor, &rightRearDriveMotor),	// these must be initialized in the same order
		stick(1),									// as they are declared above.
		stick2(2),
		gamepad(3),
		indexerMotor(INDEX_PWM),
		shooterMotor(SHOOTER_PWM),
		armMotor (ARM_PWM),
		leftDriveEncoder(LEFT_DRIVE_ENC_A, LEFT_DRIVE_ENC_B),
		rightDriveEncoder(RIGHT_DRIVE_ENC_A, RIGHT_DRIVE_ENC_B),
		shifter(SHIFTER_A,SHIFTER_B),
		greenClaw(GREEN_CLAW_LOCKED, GREEN_CLAW_UNLOCKED),
		yellowClaw(YELLOW_CLAW_LOCKED, YELLOW_CLAW_UNLOCKED),
		potentiometer(ARM_ROTATION_POT),
		indexerSwitch(INDEXER_SW),
		greenClawLockSwitch(GREEN_LOCK_SENSOR),
		yellowClawLockSwitch(YELLOW_LOCK_SENSOR),
		compressor(COMPRESSOR_PRESSURE_SW, COMPRESSOR_SPIKE),
		jogTimer(),
		shooterTimer()
	{
		m_shooterMotorRunning   = false;
		m_jogTimerRunning       = false;
		m_shiftCount            = MAX_SHIFTS;
		
		dsLCD = DriverStationLCD::GetInstance();
		dsLCD->Clear();
		dsLCD->PrintfLine(DriverStationLCD::kUser_Line1, "2013 " NAME);
		dsLCD->PrintfLine(DriverStationLCD::kUser_Line2, __DATE__ " "__TIME__);
		
#ifdef WHAMO1
		myRobot.SetInvertedMotor(RobotDrive::kRearLeftMotor, true); 
		myRobot.SetInvertedMotor(RobotDrive::kFrontLeftMotor, true); 
#endif
		
		dsLCD->UpdateLCD();
		myRobot.SetExpiration(0.1);
		shifter.Set(DoubleSolenoid::kReverse);
		
		leftDriveEncoder.SetDistancePerPulse(DRIVE_ENCODER_DISTANCE_PER_PULSE);
		leftDriveEncoder.SetMaxPeriod(1.0);
		leftDriveEncoder.SetReverseDirection(true);  // change to true if necessary
		leftDriveEncoder.Start();

	}
	
	double absolute(double value)
	{
		if (value < 0.0)
		{
			value = -1.0 * value;
		}
		return value;
	}
	
	void DoAutonomousMoveStep(const step_speed *speeds, char * message)
	{
		leftDriveEncoder.Reset();
		double dist = speeds[0].distance;
		double reading;
		
		// Start moving the robot
		myRobot.Drive(speeds->magnitude, speeds->curve);
//		leftFrontDriveMotor.Set(speeds->speed_left);
//		rightFrontDriveMotor.Set(speeds->speed_right);

		reading = absolute(leftDriveEncoder.GetDistance());
		
		while (dist > reading)
		{
			Wait(0.02);
			reading = absolute(leftDriveEncoder.GetDistance());
			dsLCD->UpdateLCD();
		}
		
		leftFrontDriveMotor.Set(0.0);
		rightFrontDriveMotor.Set(0.0);
	}

	// Shoot a single disk by cycling the indexer once and then
	// pausing to let the shooter motor spin back up to full speed.
	void DoAutonomousShootOneDisk()
	{
		// Unless the caller does an update, we will still see the openEvent
		// if we call getEvent unless we do an update here
		indexerSwitch.Update();

		indexerMotor.Set(INDEXER_FWD);

		// Wait for the indexer to cycle once
		while (IsAutonomous() && indexerSwitch.GetEvent() != kEventOpened)
		{
			Wait(0.02);
			indexerSwitch.Update();
		}

		indexerMotor.Set(0.0);

		// Let the shooter motor get back up to full speed
		Wait(1.15);	
	}

	void Autonomous(void)
	{
		myRobot.SetSafetyEnabled(false);

		// Read twice to make sure there are no false events
		// due to a comparison against a default initial value.
		indexerSwitch.Update();
		indexerSwitch.Update();

		shooterMotor.Set(SHOOTER_FWD);

		// Wait for the shooter motor to spin up to speed
		Wait(2.5);

		// Shoot the disks
		for (int i = 0; i < NUM_AUTONOMOUS_DISKS; i++)
		{
			DoAutonomousShootOneDisk();
		}
		
		shooterMotor.Set(0.0f);

		// Move the robot to a position closer to out feeder station 
		//		DoAutonomousMoveStep(&m_autoForward[0], "Backing up...");
	}
	
	void HandleDriverInputsManual(void)
	{
		myRobot.ArcadeDrive(stick.GetX(), stick.GetY());
		if(kEventClosed == stick2.GetEvent(BUTTON_SHIFT))
		{
			if (m_shiftCount)
			{
				// Shift into high gear.
				shifter.Set(DoubleSolenoid::kReverse);
				m_shiftCount--;
			}
		}
		else if(kEventOpened == stick2.GetEvent(BUTTON_SHIFT))
		{
			if (m_shiftCount)
			{
				// Shift into low gear.
				shifter.Set(DoubleSolenoid::kForward);
				m_shiftCount--;
			}
		}
	}
	
	void HandleArmInputs(void)
	{
		if (!m_jogTimerRunning)
		{
			// Climb!
			if (gamepad.GetLeftY() < -0.1)
			{
				if (potentiometer.GetVoltage() > CLIMB_LIMIT)
				{
					armMotor.Set(gamepad.GetLeftY());
				}
				else
				{
					armMotor.Set(0.0);
				}
			}
			// Descend
			else if (gamepad.GetLeftY() > 0.1)
			{
				if (potentiometer.GetVoltage() < DESCEND_LIMIT)
				{
					armMotor.Set(gamepad.GetLeftY());
				}
				else
				{
					armMotor.Set(0.0);
				}
			}
			// Climb
			else if (kEventClosed == gamepad.GetDPadEvent(Gamepad::kUp))
			{
				if (potentiometer.GetVoltage() > CLIMB_LIMIT)
				{
					armMotor.Set(ARM_CLIMB);
					jogTimer.Start();
					jogTimer.Reset();
					m_jogTimerRunning = true;
				}
				else
				{
					armMotor.Set(0.0);
				}
			}
			// Descend
			else if (kEventClosed == gamepad.GetDPadEvent(Gamepad::kDown))
			{
				if (potentiometer.GetVoltage() < DESCEND_LIMIT)
				{
					armMotor.Set(ARM_DESCEND);
					jogTimer.Start();
					jogTimer.Reset();
					m_jogTimerRunning = true;
				}
				else
				{
					armMotor.Set(0.0);
				}
			}
			else
			{
				armMotor.Set(0.0);
			}
		}
		else if (jogTimer.HasPeriodPassed(JOG_TIME))
		{
			armMotor.Set(0);
			jogTimer.Stop();
			jogTimer.Reset();
			m_jogTimerRunning = false;
		}

		if (gamepad.GetEvent(BUTTON_GREEN_CLAW_LOCKED) == kEventClosed)
		{
			greenClaw.Set(DoubleSolenoid::kForward);
		}
		else if (gamepad.GetEvent(BUTTON_GREEN_CLAW_UNLOCKED) == kEventClosed)
		{
			greenClaw.Set(DoubleSolenoid::kReverse);
		}
		else if (gamepad.GetEvent(BUTTON_YELLOW_CLAW_LOCKED) == kEventClosed)
		{
			yellowClaw.Set(DoubleSolenoid::kForward);
		}
		else if (gamepad.GetEvent(BUTTON_YELLOW_CLAW_UNLOCKED) == kEventClosed)
		{
			yellowClaw.Set(DoubleSolenoid::kReverse);
		}
	}
	
	void HandleShooterInputs()
	{
		if (indexerSwitch.GetEvent() == kEventOpened)
		{
			indexerMotor.Set(0.0);
		}
		
		if (kEventClosed == gamepad.GetEvent(BUTTON_TOGGLE_SHOOTER))
		{
			if (!m_shooterMotorRunning)
			{
				m_shooterMotorRunning = true;
				shooterMotor.Set(SHOOTER_FWD);
			}
			else
			{
				m_shooterMotorRunning = false;
				shooterMotor.Set(0.0);
			}
		}
		
		if (kEventClosed == gamepad.GetEvent(BUTTON_INDEXER))
		{
			indexerMotor.Set(INDEXER_FWD);
		}
	}
	
	void HandleResetButton(void)
	{
		if (gamepad.GetEvent(BUTTON_STOP_ALL) == kEventClosed)
		{
			shooterMotor.Set(0.0);
			m_shooterMotorRunning  = false;

			indexerMotor.Set(0.0);
			
			armMotor.Set(0.0);
			
			jogTimer.Stop();
			jogTimer.Reset();
			m_jogTimerRunning = false;
		}
	}

	void UpdateStatusDisplays(void)
	{
		// Joystick values
//		SmartDashboard::PutNumber("stickX", stick.GetX());
//		SmartDashboard::PutNumber("stickY", stick.GetY());
//		SmartDashboard::PutBoolean("shift", stick2.GetState(BUTTON_SHIFT) ? kStateClosed : kStateOpen);
		
		// Shooter/Indexer values
		SmartDashboard::PutBoolean("indexSwitch", indexerSwitch.GetState() ? kStateClosed : kStateOpen);
		SmartDashboard::PutNumber("shooterMotor", shooterMotor.Get());
		SmartDashboard::PutNumber("indexerMotor", indexerMotor.Get());

		// Misc Motor Values
//		SmartDashboard::PutNumber("armMotor", armMotor.Get());

		// Arm position via potentiometer voltage (2.5 volts is center position)
		float potVal = potentiometer.GetVoltage();
		// Checks if there has been a significant enough change in the value of
		// the arm potentiometer to warrant sending the data to the dashboard.
		if (absolute((m_previousPot - potVal)) > POT_EPSILON)
		{
			SmartDashboard::PutNumber("Potentiometer", potentiometer.GetVoltage());
			m_previousPot = potVal;
		}
		
		// Claw lock states
		SmartDashboard::PutBoolean("Green Claw State", GREEN_CLAW_LOCK_STATE);
		SmartDashboard::PutBoolean("Yellow Claw State", YELLOW_CLAW_LOCK_STATE);
		
		// Pneumatic shifter count
		SmartDashboard::PutNumber("Shift Count", m_shiftCount);
		
		// State viariables
//		dsLCD->PrintfLine(DriverStationLCD::kUser_Line6, "SMR: %s JTR: %s",
//			m_shooterMotorRunning ? "T" : "F",   
//			m_jogTimerRunning ? "T" : "F");
	}
	
	void OperatorControl(void)
	{
		myRobot.SetSafetyEnabled(false);
		
		gamepad.EnableButton(BUTTON_GREEN_CLAW_LOCKED);
		gamepad.EnableButton(BUTTON_YELLOW_CLAW_LOCKED);
		gamepad.EnableButton(BUTTON_GREEN_CLAW_UNLOCKED);
		gamepad.EnableButton(BUTTON_YELLOW_CLAW_UNLOCKED);
		gamepad.EnableButton(BUTTON_STOP_ALL);
		gamepad.EnableButton(BUTTON_JOG_FWD);
		gamepad.EnableButton(BUTTON_JOG_REV);
		gamepad.EnableButton(BUTTON_TOGGLE_SHOOTER);
		gamepad.EnableButton(BUTTON_INDEXER);

		stick2.EnableButton(BUTTON_SHIFT);

		// Set inital states for all switches and buttons
		gamepad.Update();
		indexerSwitch.Update();
		greenClawLockSwitch.Update();
		yellowClawLockSwitch.Update();
		stick2.Update();
		
		// Set initial states for all pneumatic actuators
		shifter.Set(DoubleSolenoid::kReverse);
		greenClaw.Set(DoubleSolenoid::kReverse);
		yellowClaw.Set(DoubleSolenoid::kReverse);

		compressor.Start ();
		
		while (IsOperatorControl())
		{
			gamepad.Update();
			stick2.Update();
			indexerSwitch.Update();
			greenClawLockSwitch.Update();
			yellowClawLockSwitch.Update();
			
			HandleDriverInputsManual();
			HandleArmInputs();
			HandleShooterInputs();
			HandleResetButton();
			UpdateStatusDisplays();
			
			dsLCD->PrintfLine(DriverStationLCD::kUser_Line4, "G: %s", (GREEN_CLAW_LOCK_STATE ? "Locked" : "Unlocked"));
			dsLCD->PrintfLine(DriverStationLCD::kUser_Line5, "Y: %s", (YELLOW_CLAW_LOCK_STATE ? "Locked" : "Unlocked"));
			
			dsLCD->UpdateLCD();
			Wait(0.005);				// wait for a motor update time
		}
	}
	
	/**
	 * Runs during test mode
	 */
	void Test() 
	{
	}
};

START_ROBOT_CLASS(RobotDemo);

