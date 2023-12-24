#pragma once 

typedef struct {
    int x;
    int y;
} ivec;

ivec ivec_sum(ivec a, ivec b);
ivec ivec_sub(ivec a, ivec b);
ivec ivec_mul(ivec a, int scalar);
ivec ivec_div(ivec a, int div);