#include "ivec.h"

ivec ivec_sum(ivec a, ivec b) {
    return (ivec){ a.x + b.x, a.y + b.y };
}

ivec ivec_sub(ivec a, ivec b) {
    return (ivec){ a.x - b.x, a.y - b.y };
}

ivec ivec_mul(ivec a, int scalar) {
    return (ivec){ a.x * scalar, a.y * scalar };
}

ivec ivec_div(ivec a, int div) {
    return (ivec){ a.x / div, a.y / div };
}