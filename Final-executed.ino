#include <NewPing.h>
#include <AccelStepper.h>

// Serial commands
const String START_CMD = "start";
const String STOP_CMD = "stop";

// Step state variable to track each sequential step
int currentStep = 0;
bool startFlag = false;
bool stopFlag = false;

// Motor control pins for DC motors (used in various steps)
const int in1 = 6;
const int in2 = 7;
const int in3 = 8;
const int in4 = 9;
const int enA = 10;
const int enB = 11;

// Ultrasonic sensor settings
const int trigPin = 12;
const int echoPin = 13;
const int maxDistance = 50; // Adjust as per each step's requirement
const float distanceThreshold = 2.0;

NewPing sonar(trigPin, echoPin, maxDistance);  // Initialize NewPing

// Stepper motor settings
#define motorInterfaceType 1
const int dirPinA = 4;
const int stepPinA = 5;
const int dirPinB = 2;
const int stepPinB = 3;

AccelStepper motorA(motorInterfaceType, stepPinA, dirPinA);
AccelStepper motorB(motorInterfaceType, stepPinB, dirPinB);

void setup() {
  Serial.begin(9600);

  // Motor and ultrasonic sensor pins setup
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);
  
  // Stepper motor configurations
  motorA.setMaxSpeed(1000);
  motorA.setAcceleration(500);
  motorB.setMaxSpeed(1000);
  motorB.setAcceleration(500);

  stopMotors();  // Initialize all motors to stopped state
}

void loop() {
  // Check for serial commands
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    
    if (command == START_CMD) {
      startFlag = true;
      stopFlag = false;
      Serial.println("Start command received.");
    } 
    else if (command == STOP_CMD) {
      startFlag = false;
      stopFlag = true;
      stopMotors();
      motorA.stop();
      motorB.stop();
      Serial.println("Emergency stop activated.");
    }
  }

  if (startFlag && !stopFlag) {
    executeSteps();
  }
}

void executeSteps() {
  switch (currentStep) {
    case 0:
      if (step1()) currentStep++;
      break;
    case 1:
      if (step2()) currentStep++;
      break;
    case 2:
      if (step3()) currentStep++;
      break;
    case 3:
      if (step4()) currentStep++;
      break;
    case 4:
      if (step5()) currentStep++;
      break;
    case 5:
      if (step6()) currentStep++;
      break;
    case 6:
      if (step7()) {
        currentStep = 0;  // Reset to allow the sequence to run once only
        startFlag = false;  // Reset startFlag so steps don't repeat automatically
        Serial.println("All steps completed.");
      }
      break;
  }
}

// STEP FUNCTIONS

bool step1() {
  while (getUltrasonicDistance() > distanceThreshold && !stopFlag) {
    rotateCounterClockwise();
  }
  stopMotors();
  Serial.println("Step 1 completed.");
  delay(2000);
  return true;
}

bool step2() {
  motorA.move(-70 / 0.01);  // Adjusted for 110 mm at 0.01 mm/step
  while (motorA.distanceToGo() != 0 && !stopFlag) {
    motorA.run();
  }
  Serial.println("Step 2 completed.");
  delay(2000);
  return true;
}

bool step3() {
  while (getUltrasonicDistance() > 20.0 && !stopFlag) {
    rotateCounterClockwise();
  }
  stopMotors();
  Serial.println("Step 3 completed.");
  delay(2000);
  return true;
}

bool step4() {
  motorA.move(70 / 0.01);
  motorB.move(-70 / 0.01);
  while (motorA.distanceToGo() != 0 && motorB.distanceToGo() != 0 && !stopFlag) {
    motorA.run();
    motorB.run();
  }
  Serial.println("Step 4 completed.");
  delay(2000);
  return true;
}

bool step5() {
  while (getUltrasonicDistance() > 10.0 && !stopFlag) {
    rotateCounterClockwise();
  }
  stopMotors();
  Serial.println("Step 5 completed.");
  delay(2000);
  return true;
}

bool step6() {
  motorB.move(70 / 0.01);  // Adjusted for 110 mm at 0.01 mm/step
  while (motorB.distanceToGo() != 0 && !stopFlag) {
    motorB.run();
  }
  Serial.println("Step 6 completed.");
  delay(2000);
  return true;
}

bool step7() {
  while (getUltrasonicDistance() > distanceThreshold && !stopFlag) {
    rotateCounterClockwise();
  }
  stopMotors();
  Serial.println("Step 7 completed.");
  return true;
}

// HELPER FUNCTIONS

void rotateCounterClockwise() {
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  analogWrite(enA, 255);
  analogWrite(enB, 255);
}

void stopMotors() {
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
  analogWrite(enA, 0);
  analogWrite(enB, 0);
}

float getUltrasonicDistance() {
  float distance = sonar.ping_cm();
  if (distance == 0) distance = maxDistance;
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");
  return distance;
}