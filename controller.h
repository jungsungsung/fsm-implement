#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "sensor.h"

typedef enum {
    MOTOR_FORWARD,
    MOTOR_BACKWARD,
    MOTOR_TURN_LEFT,
    MOTOR_TURN_RIGHT
} MotorCommand;

typedef enum
{
    CLEANER_ON,
    CLEANER_OFF,
    POWER_UP
} CleanerState;

typedef enum{
    ENABLE,
    DISABLE
} MoveState;


void Controller(ObstacleInfo obstacle,int dust);
void ControllerInit();

void MovingMotorInterface(MotorCommand command,MoveState state);
void TurningMotorInterface(MotorCommand command);

void MoveForward(MoveState state);
void TurnRight();
void TurnLeft();
void MoveBackward(MoveState state);

void CleanerInterface(CleanerState state);
void CleanerCommand(CleanerState state);



#endif