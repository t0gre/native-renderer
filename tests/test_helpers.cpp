#include "stdbool.h"
#include "vec.h"

bool floatsAreClose(float a, float b) {
    return fabs(a - b) < 0.00001f;
}

bool vec3sAreEqual(mym::Vec3 a, mym::Vec3 b) {
    return (floatsAreClose(a.x, b.x) && 
            floatsAreClose(a.y, b.y) && 
            floatsAreClose(a.z, b.z));
}

