#include "raycast.h"
#include "test_helpers.h"

const Triangle triangle = {
        { 1.f, 0.f, 0.1f }, 
        { 0.f, 1.f, 0.1f }, 
        {-1.f, 0.f, 0.1f } 
    };
    
TestResult intersect_triangle() {
   
    // triangle is symmetrical x-y and just a bit back from origin z
   
    const Ray ray = {
     .origin = {0.5f, 0.5f, 0.f},
     .direction = {0.f, 0.f, 1.f}
    };

    const Vec3Result result = rayIntersectsTriangle(ray, triangle);

    const Vec3Result expected = {
        .valid = true,
        .value = {0.5f, 0.5f, 0.1f}
    };

    if (!result.valid) {
        return (TestResult){
            .pass = false,
            .message = "no intersection found",
        };
    } else {
        if (vec3sAreEqual(expected.value, result.value)) {
           return (TestResult){
            .pass = true,
            .message = "correct intersection was found",
        }; 
        } else {
            return (TestResult){
            .pass = false,
            .message = "incorrect intersection found",
           
        };
        }
    }

}


TestResult dont_intersect_because_origin() {
    
 
    // pointing away from the triangle
    const Ray ray = {
        .origin = {0.5f, 0.5f, 0.2f},
        .direction = {0.f, 0.f, 1.f}
    };

    const Vec3Result result = rayIntersectsTriangle(ray, triangle);

    if (result.valid) {
         return (TestResult){
            .pass = false,
            .message = "intersection found when it should not",   
        };
    } else {
        return (TestResult){
            .pass = true,
            .message = "no intersection found, correctly",
        };
    }

}

TestResult dont_intersect_because_direction() {
    
 
    // this should intersect the triangles plane, but not the triangle itself
    const Ray ray = {
     .origin = {0.5f, 0.5f, -10.f},
     .direction = normalize((Vec3){0.f, 1.f, 1.f})
    };

    const Vec3Result result = rayIntersectsTriangle(ray, triangle);

    if (result.valid) {
         return (TestResult){
            .pass = false,
            .message = "intersection found when it should not",
           
        };
    } else {
        return (TestResult){
            .pass = true,
            .message = "no intersection found, correctly",
        };
    }

}

std::vector<TestResult> runTriangleTests() {
     
    std::vector<TestResult> results;
    results.push_back(intersect_triangle());
    results.push_back(dont_intersect_because_origin());
    results.push_back(dont_intersect_because_direction());

    return results;
}