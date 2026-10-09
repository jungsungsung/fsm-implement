#include <stdio.h>
#include "controller.h"
#include "sensor.h"
#include <windows.h>

int  main(void){

    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    ObstacleInfo obstacle;
    int dust_Existence;

    ControllerInit();

    // 1틱 = 200ms. 매 틱마다 센서를 한 번 읽고(상시 입력을 틱으로 근사) 컨트롤러를 한 번 돌린다.
    // 회전/후진 중에도 틱은 계속 돌아가므로 센서 입력이 멈추지 않는다.
    int tick = 0;

    while (1) {
        obstacle = Det_OL();
        dust_Existence = Det_DE();

        // 센서 파일을 끝까지 읽으면 종료
        if (obstacle.front < 0 || obstacle.left < 0 ||
            obstacle.right < 0 || dust_Existence < 0) {
            break;
        }

        printf("[tick %d] 앞:%d 왼:%d 오:%d 먼지:%d\n", tick,
               obstacle.front, obstacle.left, obstacle.right, dust_Existence);

        Controller(obstacle, dust_Existence);
        printf("\n");

        tick++;
        Sleep(200);
    }

    printf("시나리오 종료\n");
    return 0;
}
