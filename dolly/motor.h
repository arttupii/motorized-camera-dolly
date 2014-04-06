#ifndef __MOTOR_H__
#define __MOTOR_H__

void initializeMotor();
void runMotor();
int isMotorCalibrated();
void calibrateMotor();
void goHomeMotor();

char move_motor(unsigned long  _steps, int _direction);
char isMotorIdle();

#endif
