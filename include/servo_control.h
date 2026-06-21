#pragma once

enum ServoAngleType {
  SERVO_OPEN_ANGLE,
  SERVO_CLOSED_ANGLE,
  SERVO_FINE_ANGLE
};

void servoSetup();
void servoLoop();
void servoStartCycle();
void servoStopCycle();
bool servoIsOpening();
void servoLoadSettings();
void servoSaveSettings();
int servoGetAngle(ServoAngleType angleType);
void servoSetActiveAngle(ServoAngleType angleType);
void servoResetFineAngleToClosed();
void servoChangeActiveAngle(int delta);
void servoFinishAngleSetup();
