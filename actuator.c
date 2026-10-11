// actuator.c
#include "controller.h"
#include <stdio.h>

void MovingMotorInterface(MotorCommand command, MoveState state)
{
    switch (command)
    {
    case MOTOR_FORWARD:
        switch (state)
        {
        case ENABLE:
            printf("MovingMotorInterface : MOTOR_FORWARD,ENABLE 호출 \n");
            break;
        case DISABLE:
            printf("MovingMotorInterface : MOTOR_FORWARD,DISABLE 호출 \n");
            break;
        default:
            break;
        }
        break;
    case MOTOR_BACKWARD:
        switch (state)
        {
        case ENABLE:
            printf("MovingMotorInterface : MOTOR_BACKWARD,ENABLE 호출 \n");
            break;
        case DISABLE:
            printf("MovingMotorInterface : MOTOR_BACKWARD,DISABLE 호출 \n");
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
}

void TurningMotorInterface(MotorCommand command)
{
    switch (command)
    {
    case MOTOR_TURN_LEFT:
        printf("TurningMotorInterface : MOTOR_TURN_LEFT,ENABLE 호출 \n");
        break;
    case MOTOR_TURN_RIGHT:
        printf("TurningMotorInterface : MOTOR_TURN_RIGHT,ENABLE 호출 \n");
        break;
    default:
        break;
    }
}

void CleanerInterface(CleanerState state)
{
    switch (state)
    {
    case POWER_UP:
        printf("CleanerInterface : POWER_UP 호출 \n");

        break;
    case CLEANER_ON:
        printf("CleanerInterface : CLEANER_ON 호출 \n");

        break;
    case CLEANER_OFF:
        printf("CleanerInterface : CLEANER_OFF 호출 \n");

        break;
    default:
        break;
    }
}
