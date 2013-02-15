#include "WPILib.h"
#include "BasicDefines.h"
#include "EGamepad.h"
#include "EAnalogTrigger.h"
#include "EDigitalInput.h"
#include "Ejoystick.h"
#include "NetworkTables/NetworkTable.h"

/**
 * This is Wham-O - the 2013 2994 Robot
 */ 

class RobotDemo : public SimpleRobot
{
	RobotDrive myRobot; // robot drive system
	
	// Input Devices
	Joystick  stick; 
	EJoystick stick2;
	EGamepad  gamepad;
	
	// Motor Controllers (macros used because of differing hw on plyboy and bots)
	COLLECTOR_MOTOR_CONTROLLER collectorMotor;
	INDEXER_MOTOR_CONTROLLER   indexerMotor;
	SHOOTER_MOTOR_CONTROLLER   shooterMotor;
	ARM_MOTOR_CONTROLLER       armMotor;
	
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

	// Nonobject members
	bool m_collectorMotorRunning;
	bool m_shooterMotorRunning;
	bool m_jogTimerRunning;
	
	DriverStationLCD *dsLCD;

public:
	RobotDemo(void):
		myRobot(LEFT_DRIVE_PWM, RIGHT_DRIVE_PWM),	// these must be initialized in the same order
		stick(1),									// as they are declared above.
		stick2(2),
		gamepad(3),
		collectorMotor(PICKUP_PWM),
		indexerMotor(INDEX_PWM),
		shooterMotor(SHOOTER_PWM),
		armMotor (ARM_PWM),
		shifter(SHIFTER_A,SHIFTER_B),
		greenClaw(CLAW_1_LOCKED, CLAW_1_UNLOCKED),
		yellowClaw(CLAW_2_LOCKED, CLAW_2_UNLOCKED),
		potentiometer(ARM_ROTATION_POT),
		indexSwitch(INDEXER_SW),
		greenClawLockSwitch(CLAW_1_LOCK_SENSOR),
		yellowClawLockSwitch(CLAW_2_LOCK_SENSOR),
		compressor(COMPRESSOR_PRESSURE_SW, COMPRESSOR_SPIKE),
		jogTimer()
	{
		m_collectorMotorRunning = false;
		m_shooterMotorRunning   = false;
		m_jogTimerRunning       = false;
		
		dsLCD = DriverStationLCD::GetInstance();
		dsLCD->Clear();
		dsLCD->PrintfLine(DriverStationLCD::kUser_Line1, "2013 " NAME);
		dsLCD->PrintfLine(DriverStationLCD::kUser_Line2, __DATE__ " "__TIME__);

		dsLCD->UpdateLCD();
		myRobot.SetExpiration(0.1);
		shifter.Set(DoubleSolenoid::kReverse);
	}

	/**
	 * Drive left & right motors for 2 seconds then stop
	 */
	void Autonomous(void)
	{
		myRobot.SetSafetyEnabled(false);
		myRobot.Drive(-0.5, 0.0); 	// drive forwards half speed
		Wait(2.0); 				//    for 2 seconds
		myRobot.Drive(0.0, 0.0); 	// stop robot
	}
	
	void HandleDriverInputsManual(void)
	{
		myRobot.ArcadeDrive(stick);
		if(kEventClosed == stick2.GetEvent(BUTTON_SHIFT))
		{
			// Shift into high gear.
			shifter.Set(DoubleSolenoid::kForward);
		}
		else if(kEventOpened == stick2.GetEvent(BUTTON_SHIFT))
		{
			// Shift into low gear.
			shifter.Set(DoubleSolenoid::kReverse);
		}
	}

	void HandleDriverInputsAutomatic(void)
	{
		//myRobot.ArcadeDrive(stick);
		
		if(DoubleSolenoid::kReverse == shifter.Get())
		{
			if(stick.GetY() < -0.25)
			{
				shifter.Set(DoubleSolenoid::kForward);
			}
		}
		// If the robot is in low gear and is over 0.2 input,
		// then switch into high gear.
		else if(stick.GetY() > -0.2)
		{
			shifter.Set(DoubleSolenoid::kReverse);
		}
		
		SquareInputs();
	}
	
	void SquareInputs(void)
	{
		if(stick.GetY() < 0)
		{
			if(DoubleSolenoid::kReverse == shifter.Get())
			{
				myRobot.ArcadeDrive((stick.GetY() * stick.GetY() * -4.0), stick.GetX());
			}
			else if(DoubleSolenoid::kForward == shifter.Get())
			{
				myRobot.ArcadeDrive((stick.GetY() * stick.GetY() * -1.0), stick.GetX());
			}
		}
		else if(stick.GetY() > 0)
		{
			if(DoubleSolenoid::kReverse == shifter.Get())
			{
				myRobot.ArcadeDrive((stick.GetY() * stick.GetY() * 4.0), stick.GetX());
			}
			else if(DoubleSolenoid::kForward == shifter.Get())
			{
				myRobot.ArcadeDrive((stick.GetY() * stick.GetY() * 1.0), stick.GetX());
			}
		}
	}
	
	void HandleArmInputs(void)
	{
		if (gamepad.GetLeftY() < -0.1)
		{
			if (potentiometer.GetVoltage() < 4.5)
			{
				armMotor.Set(1.0);
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
				armMotor.Set(-1.0);
			}
			else
			{
				armMotor.Set(0.0);
			}	
		}
		else if (kEventClosed == gamepad.GetDPadEvent(Gamepad::kUp))
		{
			armMotor.Set(1.0);
			jogTimer.Start();
			jogTimer.Reset();
			m_jogTimerRunning = true;
		}
		else if (kEventClosed == gamepad.GetDPadEvent(Gamepad::kDown))
		{
			armMotor.Set(-1.0);
			jogTimer.Start();
			jogTimer.Reset();
			m_jogTimerRunning = true;
		}
//		else if (kEventClosed == gamepad.GetEvent(BUTTON_JOG_FWD))
//		{
//			armMotor.Set(1.0);
//			jogTimer.Start();
//			jogTimer.Reset();
//			m_jogTimerRunning = true;
//		}
//		else if (kEventClosed == gamepad.GetEvent(BUTTON_JOG_REV))
//		{
//			armMotor.Set(-1.0);
//			jogTimer.Start();
//			jogTimer.Reset();
//			m_jogTimerRunning = true;
//		}
		else if (m_jogTimerRunning)
		{
			if (jogTimer.HasPeriodPassed(JOG_TIME))
			{
				armMotor.Set(0);
				jogTimer.Stop();
				jogTimer.Reset();
				m_jogTimerRunning = false;
			}
		}
		else
		{
			armMotor.Set(0.0);
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
				collectorMotor.Set(1.0);
				m_collectorMotorRunning = true;
			}
			else if (kEventOpened == gamepad.GetEvent(BUTTON_COLLECTOR_FWD))
			{
				collectorMotor.Set(0.0);
				m_collectorMotorRunning = false;
			}
			else if (kEventClosed == gamepad.GetEvent(BUTTON_COLLECTOR_REV))
			{
				collectorMotor.Set(-1.0);
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
		if (!m_collectorMotorRunning && !m_shooterMotorRunning)
		{
			if (kEventClosed == gamepad.GetEvent(BUTTON_SHOOTER))
			{
				shooterMotor.Set(-0.5);
				indexerMotor.Set(-0.5);
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
		// Arm position via potentiometer voltage (2.5 volts is center position)
		dsLCD->PrintfLine(DriverStationLCD::kUser_Line3, "Voltage: %3.1f", potentiometer.GetVoltage());
		SmartDashboard::PutNumber("Potentiometer", potentiometer.GetVoltage());
		
		// Claw lock states
		SmartDashboard::PutBoolean("Green Claw State :", greenClawLockSwitch.GetState());
		SmartDashboard::PutBoolean("Yellow Claw State:", yellowClawLockSwitch.GetState());
		dsLCD->PrintfLine(DriverStationLCD::kUser_Line4, "Green : %s", 
			greenClawLockSwitch.GetState() ? "Locked" : "Unlocked");
		dsLCD->PrintfLine(DriverStationLCD::kUser_Line5, "Yellow: %s", 
			yellowClawLockSwitch.GetState() ? "Locked" : "Unlocked");
		
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

