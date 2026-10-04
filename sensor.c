#include "sensor.h"
#include <stdio.h>

//Det_OL -> 한 줄씩 장애물 정보를 읽어서 장애물 정보 구조체 전달
//Det_DE -> 한 줄씩 먼지 정보를 읽어서 먼지 정보 전달

//Determine Obstacle Location
ObstacleInfo Det_OL(void)
{
    ObstacleInfo info;

    //static으로 선언하여 다시 함수를 호출하여도 읽었던 부분에서 다시 읽을 수 있도록 함
    static FILE *front_fp = NULL;
    static FILE *left_fp = NULL;
    static FILE *right_fp = NULL;

    // 처음 호출할 때만 파일 열기
    if (front_fp == NULL) {
        front_fp = fopen("front.txt", "r");
        left_fp = fopen("left.txt", "r");
        right_fp = fopen("right.txt", "r");
    }

    info.front = ReadFront(front_fp);
    info.left = ReadLeft(left_fp);
    info.right = ReadRight(right_fp);

    return info;

}

//Front Sensor Interface
int ReadFront(FILE *front_fp){
    int value;

    if (fscanf(front_fp, "%d", &value) != 1) {
        return -1;
    }

    return value;
}

//Left Sensor Interface
int ReadLeft(FILE *left_fp){
    int value;

    if (fscanf(left_fp, "%d", &value) != 1) {
        return -1;
    }

    return value;
}

//Right Sensor Interface
int ReadRight(FILE *right_fp){
    int value;

    if (fscanf(right_fp, "%d", &value) != 1) {
        return -1;
    }

    return value;
}

//Determine Dust Existence
int Det_DE(void){
    static FILE* dust_fp = NULL;

    if(dust_fp == NULL){
        dust_fp = fopen("dust.txt","r");
    }

    return ReadDust(dust_fp);
}

//Dust Sensor Interface 
int ReadDust(FILE* dust_fp){
    int value;

    if (fscanf(dust_fp, "%d", &value) != 1) {
        return -1;
    }

    return value;
}

