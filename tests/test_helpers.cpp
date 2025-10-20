#ifndef TEST_HELPERS_H
#define TEST_HELPERS_H

#include "stdbool.h"
#include "vec.h"

typedef struct TestResult {
    bool pass;
    const char * message;
} TestResult;


bool floatsAreClose(float a, float b) {
    return fabs(a - b) < 0.00001f;
}

bool vec3sAreEqual(Vec3 a, Vec3 b) {
    return (floatsAreClose(a.x, b.x) && 
            floatsAreClose(a.y, b.y) && 
            floatsAreClose(a.z, b.z));
}

#endif