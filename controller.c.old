#include "controller.h"
#include <windows.h>
#include <stdio.h>

// 뒤로 가고 있는 상태 저장
static int isMoveBackward = 0;

// powerUp 상태가 몇 틱 남았는지 저장
static int powerUpTick = 0;

// fsm 첫 작동 시 초기화
void ControllerInit(){
    MoveForward(ENABLE);
    CleanerCommand(CLEANER_ON);
}

void Controller(ObstacleInfo obstacle, int dust)
{
    int isFront = obstacle.front;
    int isLeft = obstacle.left;
    int isRight = obstacle.right;
    int isDust = dust;

    // 앞으로 몇 틱 동안 더 돌아야하는지 저장
    int turnTicks = 0;
    // 앞으로 몇 틱 동안 뒤로 가야하는지 저장
    int moveBackTicks = 0;

    if (isDust && !isFront && powerUpTick == 0)
    {
        CleanerCommand(POWER_UP);
        powerUpTick = 5;
    }

    if (powerUpTick)
    {
        powerUpTick--;
        
        //파워 업 상태가 끝나면 클리너 온
        if(powerUpTick == 0){
            CleanerCommand(CLEANER_ON);
        }

        //파워 업 상태에서 뒤로 가기가 끝나고 어디로 돌지 정함
        if (isMoveBackward)
        {
            if (!isRight)
            {
                MoveBackward(DISABLE);
                isMoveBackward = 0;

                TurnRight();
                turnTicks = 5;

                // 트리거를 켜놓고 5틱동안 기다리기
                while (turnTicks)
                {
                    turnTicks--;
                    Sleep(200);
                }

                CleanerCommand(CLEANER_ON);
                MoveForward(ENABLE);
            }
            else if (!isLeft)
            {
                MoveBackward(DISABLE);
                isMoveBackward = 0;

                TurnLeft();
                turnTicks = 5;

                // 트리거를 켜놓고 5틱동안 기다리기
                while (turnTicks)
                {
                    turnTicks--;
                    Sleep(200);
                }

                CleanerCommand(CLEANER_ON);
                MoveForward(ENABLE);
            }
            return;
        }

        //파워 업 상태에서 장애물을 만났을 때
        if (isFront)
        {
            if (!isRight)
            {
                powerUpTick = 0;
                CleanerCommand(CLEANER_OFF);
                MoveForward(DISABLE);

                TurnRight();
                turnTicks = 5;

                // 트리거를 켜놓고 5틱동안 기다리기
                while (turnTicks)
                {
                    turnTicks--;
                    Sleep(200);
                }

                CleanerCommand(CLEANER_ON);
                MoveForward(ENABLE);
            }
            else if (!isLeft)
            {
                powerUpTick = 0;
                CleanerCommand(CLEANER_OFF);
                MoveForward(DISABLE);

                TurnLeft();
                turnTicks = 5;

                // 트리거를 켜놓고 5틱동안 기다리기
                while (turnTicks)
                {
                    turnTicks--;
                    Sleep(200);
                }

                CleanerCommand(CLEANER_ON);
                MoveForward(ENABLE);
            }
            else
            {
                powerUpTick = 0;
                CleanerCommand(CLEANER_OFF);
                MoveForward(DISABLE);

                MoveBackward(ENABLE);
                isMoveBackward = 1;

                moveBackTicks = 3;
                while (moveBackTicks)
                {
                    moveBackTicks--;
                    Sleep(200);
                }
            }
            return;
        }
    }

    //뒤로 가기가 끝났을 때 어디로 돌지 정함
    if (isMoveBackward)
    {
        if (!isRight)
        {
            MoveBackward(DISABLE);
            isMoveBackward = 0;

            TurnRight();
            turnTicks = 5;

            // 트리거를 켜놓고 5틱동안 기다리기
            while (turnTicks)
            {
                turnTicks--;
                Sleep(200);
            }

            CleanerCommand(CLEANER_ON);
            MoveForward(ENABLE);
        }
        else if (!isLeft)
        {
            MoveBackward(DISABLE);
            isMoveBackward = 0;

            TurnLeft();
            turnTicks = 5;

            // 트리거를 켜놓고 5틱동안 기다리기
            while (turnTicks)
            {
                turnTicks--;
                Sleep(200);
            }

            CleanerCommand(CLEANER_ON);
            MoveForward(ENABLE);
        }
        return;
    }

    //장애물을 만났을 경우
    if (isFront)
    {
        if (!isRight)
        {
            CleanerCommand(CLEANER_OFF);
            MoveForward(DISABLE);

            TurnRight();
            turnTicks = 5;

            // 트리거를 켜놓고 5틱동안 기다리기
            while (turnTicks)
            {
                turnTicks--;
                Sleep(200);
            }

            CleanerCommand(CLEANER_ON);
            MoveForward(ENABLE);
        }
        else if (!isLeft)
        {
            CleanerCommand(CLEANER_OFF);
            MoveForward(DISABLE);

            TurnLeft();
            turnTicks = 5;

            // 트리거를 켜놓고 5틱동안 기다리기
            while (turnTicks)
            {
                turnTicks--;
                Sleep(200);
            }

            CleanerCommand(CLEANER_ON);
            MoveForward(ENABLE);
        }
        else
        {
            CleanerCommand(CLEANER_OFF);
            MoveForward(DISABLE);

            MoveBackward(ENABLE);
            isMoveBackward = 1;

            moveBackTicks = 3;
            while (moveBackTicks)
            {
                moveBackTicks--;
                Sleep(200);
            }
        }
    }
}

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

void MoveForward(MoveState state)
{

    switch (state)
    {
    case ENABLE:
        printf("MoveForward : ENABLE 호출\n");
        MovingMotorInterface(MOTOR_FORWARD, state);
        break;
    case DISABLE:
        printf("MoveForward : DISABLE 호출\n");
        MovingMotorInterface(MOTOR_FORWARD, state);
        break;
    default:
        break;
    }
}
void TurnRight()
{
    printf("TurnRight 호출\n");
    TurningMotorInterface(MOTOR_TURN_RIGHT);
}
void TurnLeft()
{
    printf("TurnLeft 호출\n");
    TurningMotorInterface(MOTOR_TURN_LEFT);
}
void MoveBackward(MoveState state)
{
    switch (state)
    {
    case ENABLE:
        printf("MoveBackward : ENABLE 호출\n");
        MovingMotorInterface(MOTOR_BACKWARD, state);
        break;
    case DISABLE:
        printf("MoveBackward : DISABLE 호출\n");
        MovingMotorInterface(MOTOR_BACKWARD, state);
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

void CleanerCommand(CleanerState state)
{
    switch (state)
    {
    case POWER_UP:
        printf("CleanerCommand : POWER_UP 호출 \n");
        CleanerInterface(POWER_UP);

        break;
    case CLEANER_ON:
        printf("CleanerCommand : CLEANER_ON 호출 \n");
        CleanerInterface(CLEANER_ON);

        break;
    case CLEANER_OFF:
        printf("CleanerCommand : CLEANER_OFF 호출 \n");
        CleanerInterface(CLEANER_OFF);

        break;
    default:
        break;
    }
}
