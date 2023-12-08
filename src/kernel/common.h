#pragma once

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef int int32;
typedef char int8;
typedef short int16;

#define RAND_MAX 32767

uint16 strlen(char* str);
void int_to_string(int32 n, char* str);
uint32 abs(int32 n);
void memcpy(uint8* dest, uint8* src, uint32 size);
void memset(uint8* dest, uint8 byte, uint32 size);
void* malloc(uint32 n);
void malloc_init();

uint32 trand();
uint32 rand();
void srand(uint32 seed);
float rand_float(float f1, float f2);
uint32 rand_int(uint32 a, uint32 b);