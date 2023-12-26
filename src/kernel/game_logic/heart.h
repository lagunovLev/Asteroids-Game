#pragma once 
#include "../common.h"
#include "../drivers/video.h"

extern int heart_size_x;
extern int heart_size_y;
extern char heart[];

void put_heart(int16 x, int16 y, uint8 color);