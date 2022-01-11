#include <AccelStepper.h>

/* Electric standing desk controller
 * Lukasz Ziolkowski
 * 
 * Independently control left and right motors or drive both 
 * motors together based on the position of the 3-way toggle switch.
 * Independent control is implemented to level out the desk incase of
 * misalignment.
 * 
 * The desk is driven in the direction of the up/down pushbuttons.
 * 
 */

// Pinouts are for NodeMCU (ESP-12E)
// Assign pinouts for input buttons
const int upButtonPin = 5;                 // D1 
const int downButtonPin = 10;              // SD3
const int leftMotorTogglePin = 4;          // D2
const int rightMotorTogglePin = 14;        // D5

// Assign pinouts for motor driver
const int motorDriverDirectionPin = 9;     // SD2
const int motorDriverPulsePin = 13;        // D7
const int leftMotorDriverEnablePin = 12;   // D3
const int rightMotorDriverEnablePin = 15;  // D8

// Track the current and previous input button/toggle switch states
int upButtonState;
int downButtonState;
int leftToggleState;
int rightToggleState;

int prevUpButtonState;
int prevDownButtonState;

int driveMotorUp;
int driveMotorDown;

// Motor driver interface between stepper motors and the MCU
#define motorInterfaceType 1

// Only one stepperMotor instance is used. Both stepper motors are 
// controlled by enabling/disabling the motor driver.
AccelStepper stepperMotor = AccelStepper(motorInterfaceType, motorDriverPulsePin, motorDriverDirectionPin);

void setup() {

  // Assign pinouts
  pinMode(upButtonPin, INPUT);
  pinMode(downButtonPin, INPUT);
  pinMode(leftMotorTogglePin, INPUT);
  pinMode(rightMotorTogglePin, INPUT);

  pinMode(motorDriverDirectionPin, OUTPUT);
  pinMode(motorDriverPulsePin, OUTPUT);
  pinMode(rightMotorDriverEnablePin, OUTPUT);
  pinMode(leftMotorDriverEnablePin, OUTPUT);

  // Set stepper motor speeds and acceleration
  stepperMotor.setMaxSpeed(12000);
  stepperMotor.setAcceleration(4000);
}

void loop() {

  // Read button states
  upButtonState = digitalRead(upButtonPin);
  downButtonState = digitalRead(downButtonPin);
  leftToggleState = digitalRead(leftMotorTogglePin);
  rightToggleState = digitalRead(rightMotorTogglePin);

  
  if (upButtonState != prevUpButtonState)  {
    if (upButtonState)    {
      driveMotorUp = 1;
      stepperMotor.setCurrentPosition(0);
      stepperMotor.moveTo(64000000);
      digitalWrite(motorDriverDirectionPin, HIGH);
    }
    else    {
      driveMotorUp = 0;
      stepperMotor.stop();
    }
  }

  if (downButtonState != prevDownButtonState)  {
    if (downButtonState)    {
      driveMotorDown = 1;
      stepperMotor.setCurrentPosition(0);
      stepperMotor.moveTo(-64000000);
    }
    else    {
      driveMotorDown = 0;
      stepperMotor.stop();
    }
  }  
  
  prevUpButtonState = upButtonState;
  prevDownButtonState = downButtonState;

  if (driveMotorUp || driveMotorDown) {
    if (leftToggleState)
      digitalWrite(rightMotorDriverEnablePin, HIGH);
    else
      digitalWrite(rightMotorDriverEnablePin, LOW);
  
    if (rightToggleState)
      digitalWrite(leftMotorDriverEnablePin, HIGH);
    else
      digitalWrite(leftMotorDriverEnablePin, LOW);   
  }
  else {

      // If up/down button is not enabled, disable motor drivers.
      digitalWrite(rightMotorDriverEnablePin, HIGH);
      digitalWrite(leftMotorDriverEnablePin, HIGH);
  }

  stepperMotor.run();
}
