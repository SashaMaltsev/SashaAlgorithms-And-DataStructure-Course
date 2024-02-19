#include <limits.h>

#include <iostream>

int Sum(int a, int b) {

    if ((b > 0) and (INT_MAX - b < a)) {
        return a;
    }

    if ((b < 0) and (INT_MIN - b > a)) {
        return a;
    }

    return a + b;
}
