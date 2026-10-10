#include "controller.h"
#include <stdio.h>

// 각 동작이 몇 틱 동안 이어지는지 (1틱 = main의 루프 한 바퀴)
#define TURN_TICKS     5
#define BACK_TICKS     3
#define POWERUP_TICKS  5

// FSM 상태
typedef enum {
    ST_FORWARD,   // 전진하며 청소
    ST_TURNING,   // 회전 중
    ST_BACKWARD   // 후진 중
} RobotState;

static RobotState state = ST_FORWARD;

// 현재 상태(회전/후진)가 몇 틱 남았는지
static int stateTicks = 0;

// powerUp 상태가 몇 틱 남았는지 저장
static int powerUpTick = 0;

// fsm 첫 작동 시 초기화
void ControllerInit(){
    state = ST_FORWARD;
    stateTicks = 0;
    powerUpTick = 0;
    MoveForward(ENABLE);
    CleanerCommand(CLEANER_ON);
}

// 회전 시작: 기다리지 않고 남은 틱만 설정하고 바로 리턴
static void StartTurn(int toRight)
{
    if (toRight) TurnRight();
    else         TurnLeft();
    state = ST_TURNING;
    stateTicks = TURN_TICKS;
}

// 매 틱마다 한 번 호출된다. 내부에서 Sleep 하지 않는다.
void Controller(ObstacleInfo obstacle, int dust)
{
    int isFront = obstacle.front;
    int isLeft = obstacle.left;
    int isRight = obstacle.right;
    int isDust = dust;

    //파워 업 상태가 끝나면 클리너 온
    if (powerUpTick > 0) {
        powerUpTick--;
        if (powerUpTick == 0) {
            CleanerCommand(CLEANER_ON);
        }
    }

    switch (state)
    {
    case ST_TURNING:
        stateTicks--;
        if (stateTicks > 0) {
            return;  // 아직 회전 중
        }
        // 회전 완료 -> 전진 재개
        CleanerCommand(CLEANER_ON);
        MoveForward(ENABLE);
        state = ST_FORWARD;
        // 회전이 끝난 틱의 센서 값으로 바로 판단 (앞이 또 막혀 있을 수 있음)
        // fall through

    case ST_FORWARD:
        //장애물을 만났을 경우 (파워 업 중이어도 취소)
        if (isFront)
        {
            powerUpTick = 0;
            CleanerCommand(CLEANER_OFF);
            MoveForward(DISABLE);

            if (!isRight) {
                StartTurn(1);
            }
            else if (!isLeft) {
                StartTurn(0);
            }
            else {
                MoveBackward(ENABLE);
                state = ST_BACKWARD;
                stateTicks = BACK_TICKS;
            }
        }
        //먼지를 만났을 경우
        else if (isDust && powerUpTick == 0)
        {
            CleanerCommand(POWER_UP);
            powerUpTick = POWERUP_TICKS;
        }
        break;

    case ST_BACKWARD:
        if (stateTicks > 0) {
            stateTicks--;
        }
        if (stateTicks > 0) {
            return;  // 최소 후진 시간 동안은 계속 후진
        }
        //뒤로 가기가 끝났을 때 어디로 돌지 정함
        if (!isRight) {
            MoveBackward(DISABLE);
            StartTurn(1);
        }
        else if (!isLeft) {
            MoveBackward(DISABLE);
            StartTurn(0);
        }
        // 양옆이 다 막혀 있으면 계속 후진하고, 다음 틱에 다시 확인
        break;
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
