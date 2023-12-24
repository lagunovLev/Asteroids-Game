#include "math.h"

double dmin(double a, double b) {
    return a < b ? a : b;
}

double dmax(double a, double b) {
    return a > b ? a : b;
}

int min(int a, int b) {
  return a < b ? a : b;
}

int max(int a, int b) {
  return a > b ? a : b;
}

int factorial(int n) {
    if (n == 0) {
        return 1;
    }

    return n * factorial(n - 1);
}

double pow(double base, int exponent) {
  double result = 1;
  for (int i = 0; i < exponent; i++) {
    result *= base;
  }

  return result;
}

double sign(double x) {
  if (x > 0) {
    return 1;
  } else if (x < 0) {
    return -1;
  } else {
    return 0;
  }
}

double sqrt(double n)
{
    // Max and min are used to take into account numbers less than 1
    double lo = dmin(1, n), hi = dmax(1, n), mid;

    // Update the bounds to be off the target by a factor of 10
    while(100 * lo * lo < n) lo *= 10;
    while(0.01 * hi * hi > n) hi *= 0.1;

    for(int i = 0 ; i < 100 ; i++){
        mid = (lo+hi)/2;
        if(mid*mid == n) return mid;
        if(mid*mid > n) hi = mid;
        else lo = mid;
    }
    return mid;
}

double fabs(double x) {
    return x > 0 ? x : -x;
}

uint32 abs(int32 x) {
    return x > 0 ? x : -x;
}