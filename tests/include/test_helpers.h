#pragma once

#include "stdbool.h"
#include "vec.h"
#include <vector>

struct TestResult {
    bool pass;
    const char * message;
};

bool floatsAreClose(float a, float b);
bool vec3sAreEqual(mym::Vec3 a, mym::Vec3 b);

std::vector<TestResult> runTriangleTests();
std::vector<TestResult> runVerticesTests();
std::vector<TestResult> runSceneTests();