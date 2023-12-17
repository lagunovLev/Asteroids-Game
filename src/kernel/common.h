#pragma once
#include <stdarg.h>

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef int int32;
typedef char int8;
typedef short int16;

#define RAND_MAX 4294967295
#define NULL 0

uint16 strlen(char* str);
void int_to_string(int32 n, char* str);
void float_to_string(double n, char* str);
void hex_to_string(int32 n, char* str);
void memcpy(uint8* dest, uint8* src, uint32 size);
void memset(uint8* dest, uint8 byte, uint32 size);
void* malloc(uint32 n);
void free(void* ptr);
void malloc_init();
void malloc_destruct();

uint32 trand();
uint32 rand();
void srand(uint32 seed);
float rand_float(float f1, float f2);
uint32 rand_int(uint32 a, uint32 b);

void sprintf(char *s, const char *format, ...);
void vsprintf(char *s, const char *format, va_list ap);