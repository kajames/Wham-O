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
	LEFT_DRIVE_MOTOR leftDriveMotor; // Done BEFORE the robot drive which uses them
	RIGHT_DRIVE_MOTOR rightDriveMotor;
	
	// Drive system
	RobotDrive myRobot; // robot drive system
	
	// Input Devices
	Joystick  stick; 
	EJoystick stick2;
	EGamepad  gamepad;
	
	// Other Motor Controllers (macros used because of differing hw on plyboy and bots)
	COLLECTOR_MOTOR_CONTROLLER collectorMotor;
	INDEXER_MOTOR_CONTROLLER   indexerMotor;
	SHOOTER_MOTOR_CONTROLLER   shooterMotor;
	ARM_MOTOR_CONTROLLER       armMotor;
	
	// Shaft encoders
	Encoder leftDriveEncoder;
	
	// Output Devices
	DoubleSolenoid shifter;
	DoubleSolenoid greenClaw;
	DoubleSolenoid yellowClaw;
	
	// Input sensors
	AnalogChannel potentiometer;
	EDigitalInput indexSwitch;
	EDigitalInput greenClawLockSwitch;
	EDigitalInput yellowClawLockSwitch;
	
	// Miscellaneous
	Compressor compressor;
	Timer jogTimer;
	Timer shooterTimer;

	// Nonobject members
	bool m_collectorMotorRunning;
	bool m_shooterMotorRunning;
	bool m_jogTimerRunning;
	int  m_shiftCount;
	
	DriverStationLCD *dsLCD;

public:
	RobotDemo(void):
		leftDriveMotor(LEFT_DRIVE_PWM),
		rightDriveMotor(RIGHT_DRIVE_PWM),
		myRobot(&leftDriveMotor, &rightDriveMotor),	// these must be initialized in the same order
		stick(1),									// as they are declared above.
		stick2(2),
		gamepad(3),
		collectorMotor(PICKUP_PWM),
		indexerMotor(INDEX_PWM),
		shooterMotor(SHOOTER_PWM),
		armMotor (ARM_PWM),
		leftDriveEncoder(LEFT_DRIVE_ENC_A, LEFT_DRIVE_ENC_B),
		shifter(SHIFTER_A,SHIFTER_B),
		greenClaw(CLAW_1_LOCKED, CLAW_1_UNLOCKED),
		yellowClaw(CLAW_2_LOCKED, CLAW_2_UNLOCKED),
		potentiometer(ARM_ROTATION_POT),
		indexSwitch(INDEXER_SW),
		greenClawLockSwitch(CLAW_1_LOCK_SENSOR),
		yellowClawLockSwitch(CLAW_2_LOCK_SENSOR),
		compressor(COMPRESSOR_PRESSURE_SW, COMPRESSOR_SPIKE),
		jogTimer(),
		shooterTimer()
	{
		m_collectorMotorRunning = false;
		m_shooterMotorRunning   = false;
		m_jogTimerRunning       = false;
		m_shiftCount            = MAX_SHIFTS;
		
		dsLCD = DriverStationLCD::GetInstance();
		dsLCD->Clear();
		dsLCD->PrintfLine(DriverStationLCD::kUser_Line1, "2013 " NAME);
		dsLCD->PrintfLine(DriverStationLCD::kUser_Line2, __DATE__ " "__TIME__);

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
		leftDriveMotor.Set(speeds->speed_left);
		rightDriveMotor.Set(speeds->speed_right);

		reading = absolute(leftDriveEncoder.GetDistance());
		
		while (dist > reading)
		{
			Wait(0.02);
			reading = absolute(leftDriveEncoder.GetDistance());
			dsLCD->PrintfLine(DriverStationLCD::kUser_Line3, "D: %5.0f R : %5.0f", dist, reading);
			dsLCD->UpdateLCD();
		}
		
		leftDriveMotor.Set(0.0);
		rightDriveMotor.Set(0.0);
	}

	void Autonomous(void)
	{
		myRobot.SetSafetyEnabled(false);
		
		// Move the arm to a level position
		armMotor.Set(ARM_FWD);
		while (!((potentiometer.GetVoltage() > 2.4) && (potentiometer.GetVoltage() < 2.6))) 
		{
			// Check to make sure we don't overrotate the arm in either direction
			if (potentiometer.GetVoltage() > 4.5 || potentiometer.GetVoltage() < 0.5)
			{
				armMotor.Set(0.0);
				return;
			}
			Wait(0.2);
			UpdateStatusDisplays();
		}
		armMotor.Set(0.0);
		
		// Drive robot 
		
		DoAutonomousMoveStep(&m_autoForward[0], "Moving...");
		DoAutonomousMoveStep(&m_autoForward[1], "Moving...");
		
		//Shoot for five seconds
		Timer* t = new Timer();
		t->Start();
		
		shooterMotor.Set(SHOOTER_FWD);
		indexerMotor.Set(INDEXER_FWD);

		while(!t->HasPeriodPassed(5.0))
		{
			Wait(0.02);
		}
		shooterMotor.Set(0.0);
		indexerMotor.Set(0.0);
	}
	
	void HandleDriverInputsManual(void)
	{
		myRobot.ArcadeDrive(stick);
		if(kEventClosed == stick2.GetEvent(BUTTON_SHIFT))
		{
			if (m_shiftCount)
			{
				// Shift into high gear.
				shifter.Set(DoubleSolenoid::kForward);
				m_shiftCount--;
			}
		}
		else if(kEventOpened == stick2.GetEvent(BUTTON_SHIFT))
		{
			if (m_shiftCount)
			{
				// Shift into low gear.
				shifter.Set(DoubleSolenoid::kReverse);
				m_shiftCount--;
			}
		}
	}

	void HandleDriverInputsAutomatic(void)
	{
		// Placeholder for automatic transmission code
	}
	
	void HandleArmInputs(void)
	{
		if (!m_jogTimerRunning)
		{
			if (gamepad.GetLeftY() < -0.1)
			{
				if (potentiometer.GetVoltage() < 4.5)
				{
					armMotor.Set(ARM_FWD);
				}
				else
				{
					armMotor.Set(0.0);
				}
			}
			else if (gamepad.GetLeftY() > 0.1)
			{
				if (potentiometer.GetVoltage() > .5)
				{
					armMotor.Set(ARM_REV);
				}
				else
				{
					armMotor.Set(0.0);
				}	
			}
			else if (kEventClosed == gamepad.GetDPadEvent(Gamepad::kUp))
			{
				armMotor.Set(ARM_FWD);
				jogTimer.Start();
				jogTimer.Reset();
				m_jogTimerRunning = true;
			}
			else if (kEventClosed == gamepad.GetDPadEvent(Gamepad::kDown))
			{
				armMotor.Set(ARM_REV);
				jogTimer.Start();
				jogTimer.Reset();
				m_jogTimerRunning = true;
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

		if (gamepad.GetEvent(BUTTON_CLAW_1_LOCKED) == kEventClosed)
		{
			greenClaw.Set(DoubleSolenoid::kForward);
		}
		else if (gamepad.GetEvent(BUTTON_CLAW_1_UNLOCKED) == kEventClosed)
		{
			greenClaw.Set(DoubleSolenoid::kReverse);
		}
		else if (gamepad.GetEvent(BUTTON_CLAW_2_LOCKED) == kEventClosed)
		{
			yellowClaw.Set(DoubleSolenoid::kForward);
		}
		else if (gamepad.GetEvent(BUTTON_CLAW_2_UNLOCKED) == kEventClosed)
		{
			yellowClaw.Set(DoubleSolenoid::kReverse);
		}
		
	}
	
	// This method reads two buttons on the gamepad: one for forward motion of
	// the collector (to gather disks) and one for reverse motion (to enable
	// removal of disks from the collector without having to shoot them). The
	// buttons must be held down the entire time the collector is in operation.
	// This is done to ensure that the collector is always in a known state
	// (doing nothing) if the controls are released.
	
	void HandleCollectorInputs ()
	{		
		if (false == m_shooterMotorRunning)
		{
			if (kEventClosed == gamepad.GetEvent(BUTTON_COLLECTOR_FWD))
			{
				collectorMotor.Set(COLLECTOR_FWD);
				m_collectorMotorRunning = true;
			}
			else if (kEventOpened == gamepad.GetEvent(BUTTON_COLLECTOR_FWD))
			{
				collectorMotor.Set(0.0);
				m_collectorMotorRunning = false;
			}
			else if (kEventClosed == gamepad.GetEvent(BUTTON_COLLECTOR_REV))
			{
				collectorMotor.Set(COLLECTOR_REV);
				m_collectorMotorRunning = true;
			}
			else if (kEventOpened == gamepad.GetEvent(BUTTON_COLLECTOR_REV))
			{
				collectorMotor.Set(0.0);
				m_collectorMotorRunning = false;
			}
		}
	}
	
	void HandleShooterInputs()
	{	
		if (shooterTimer.HasPeriodPassed(SPINUP_TIME))
		{
			shooterTimer.Stop();
			shooterTimer.Reset();
			
			indexerMotor.Set(INDEXER_FWD);
		}
		else if (!m_collectorMotorRunning && !m_shooterMotorRunning)
		{
			if (kEventClosed == gamepad.GetEvent(BUTTON_SHOOTER))
			{
				shooterMotor.Set(SHOOTER_FWD);
				shooterTimer.Start();
				m_shooterMotorRunning  = true;
			}
		}
		else	
		{
			if (indexSwitch.GetEvent() == kEventOpened)
			{
				indexerMotor.Set(0.0);
				shooterMotor.Set(0.0);
				m_shooterMotorRunning  = false;
			}
		}		
	}
	
	void HandleResetButton(void)
	{
		if (gamepad.GetEvent(BUTTON_STOP_ALL) == kEventClosed)
		{
			collectorMotor.Set(0.0);
			m_collectorMotorRunning = false;

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
		SmartDashboard::PutNumber("stickX", stick.GetX());
		SmartDashboard::PutNumber("stickY", stick.GetY());
		SmartDashboard::PutBoolean("shift", stick2.GetState(BUTTON_SHIFT) ? kStateClosed : kStateOpen);
		
		// Shooter/Indexer values
		SmartDashboard::PutBoolean("indexSwitch", indexSwitch.GetState() ? kStateClosed : kStateOpen);
		SmartDashboard::PutNumber("shooterMotor", shooterMotor.Get());
		SmartDashboard::PutNumber("indexerMotor", indexerMotor.Get());

		// Misc Motor Values
		SmartDashboard::PutNumber("collectorMotor", collectorMotor.Get());
		SmartDashboard::PutNumber("armMotor", armMotor.Get());

		// Arm position via potentiometer voltage (2.5 volts is center position)
		dsLCD->PrintfLine(DriverStationLCD::kUser_Line3, "Voltage: %3.1f", potentiometer.GetVoltage());
		SmartDashboard::PutNumber("Potentiometer", potentiometer.GetVoltage());
		
		// Claw lock states
		SmartDashboard::PutBoolean("Green Claw State", greenClawLockSwitch.GetState());
		SmartDashboard::PutBoolean("Yellow Claw State", yellowClawLockSwitch.GetState());
		dsLCD->PrintfLine(DriverStationLCD::kUser_Line4, "Green : %s", 
			greenClawLockSwitch.GetState() ? "Locked" : "Unlocked");
		dsLCD->PrintfLine(DriverStationLCD::kUser_Line5, "Yellow: %s", 
			yellowClawLockSwitch.GetState() ? "Locked" : "Unlocked");
		
		// Pneumatic shifter count
		SmartDashboard::PutNumber("Shift Count", m_shiftCount);
		
		// State viariables
		dsLCD->PrintfLine(DriverStationLCD::kUser_Line6, "CMR: %s SMR: %s JTR: %s",
			m_collectorMotorRunning ? "T" : "F",
			m_shooterMotorRunning ? "T" : "F",   
			m_jogTimerRunning ? "T" : "F");
	}
	
	void OperatorControl(void)
	{
		myRobot.SetSafetyEnabled(true);
		
		gamepad.EnableButton(BUTTON_COLLECTOR_FWD);
		gamepad.EnableButton(BUTTON_COLLECTOR_REV);
		gamepad.EnableButton(BUTTON_SHOOTER);
		gamepad.EnableButton(BUTTON_CLAW_1_LOCKED);
		gamepad.EnableButton(BUTTON_CLAW_2_LOCKED);
		gamepad.EnableButton(BUTTON_CLAW_1_UNLOCKED);
		gamepad.EnableButton(BUTTON_CLAW_2_UNLOCKED);
		gamepad.EnableButton(BUTTON_STOP_ALL);
		gamepad.EnableButton(BUTTON_JOG_FWD);
		gamepad.EnableButton(BUTTON_JOG_REV);

		stick2.EnableButton(BUTTON_SHIFT);

		// Set inital states for all switches and buttons
		gamepad.Update();
		indexSwitch.Update();
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
			indexSwitch.Update();
			greenClawLockSwitch.Update();
			yellowClawLockSwitch.Update();
			
			HandleCollectorInputs();
			HandleDriverInputsManual();
			HandleArmInputs();
			HandleShooterInputs();
			HandleResetButton();
			UpdateStatusDisplays();
			
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

