#include "vec.h"
#include "../math.h"

vec vec_sum(vec a, vec b) {
    return (vec){ a.x + b.x, a.y + b.y };
}

vec vec_sub(vec a, vec b) {
    return (vec){ a.x - b.x, a.y - b.y };
}

vec vec_mul(vec a, float scalar) {
    return (vec){ a.x * scalar, a.y * scalar };
}

float vec_length(vec a) {
    return sqrt(a.x * a.x + a.y * a.y);
}

float vec_distance(vec a, vec b) {
    return vec_length(vec_sub(a, b));
}

vec vec_norm(vec a) {
    float len = vec_length(a);
    return (vec){ a.x / len, a.y / len };
}

float vec_scalar(vec a, vec b) {
    return a.x * b.x + a.y * b.y;
}