#ifndef SENSOR_H
#define SENSOR_H

#include <stdio.h>

//장애물 위치 구조체
typedef struct {
    int front;
    int left;
    int right;
} ObstacleInfo;

// 1.1-1.4  Sensor Interface
int ReadFront(FILE* fp);
int ReadLeft(FILE* fp);
int ReadRight(FILE* fp);
int ReadDust(FILE* fp);

//위,왼쪽,오른쪽 장애물을 확인 후 구조체를 반환
ObstacleInfo Det_OL(void);

//먼지 정보를 읽어서 있으면 1 없으면 0을 반환
int Det_DE(void);

#endif 
