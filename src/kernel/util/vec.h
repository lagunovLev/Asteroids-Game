#pragma once

typedef struct {
    float x;
    float y;
} vec;

vec vec_sum(vec a, vec b);
vec vec_sub(vec a, vec b);
vec vec_mul(vec a, float scalar);
float vec_length(vec a);
float vec_distance(vec a, vec b);
vec vec_norm(vec a);
float vec_scalar(vec a, vec b);
