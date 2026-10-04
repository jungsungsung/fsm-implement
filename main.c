#include <stdio.h>
#include "controller.h"
#include "sensor.h"
#include <windows.h>

void main(){
    ObstacleInfo obstacle;
    int dust_Existence;

    ControllerInit();

    while(1){
        obstacle = Det_OL();
        dust_Existence = Det_DE();

        Controller(obstacle,dust_Existence);

        Sleep(200);
    }
}