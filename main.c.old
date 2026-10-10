#include <stdio.h>
#include "controller.h"
#include "sensor.h"
#include <windows.h>

int  main(void){

    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    //printf("ㅎㅇ");

    ObstacleInfo obstacle;
    int dust_Existence;

    ControllerInit();
    
    int tick = 8;

    while(tick--){
        obstacle = Det_OL();
        dust_Existence = Det_DE();

        Controller(obstacle,dust_Existence);
        printf("\n");

        Sleep(200);
    }
}