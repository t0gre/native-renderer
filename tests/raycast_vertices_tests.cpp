#include "mesh.h"
#include "test_helpers.cpp"
#include <raycast.h>

float pos_dat[18] = {
    -10.f, 0.f, -10.f, // back left
    -10.f, 0.f, 10.f, // front left
     10.f, 0.f, -10.f, // back right
    -10.f, 0.f, 10.f, // front left
     10.f, 0.f, 10.f, // front right
     10.f, 0.f, -10.f, // back right
     };
     
float norm_dat[18] = {
        0.f,1.f, 0.f,
        0.f,1.f, 0.f,
        0.f,1.f, 0.f,
        0.f,1.f, 0.f,
        0.f,1.f, 0.f,
        0.f,1.f, 0.f,
};

const Vertices meshVertices = {
    .vertex_count = 6,
    .positions = pos_dat,
    .normals = norm_dat,
  
}; 
        

TestResult intersect_vertices_first() {
   
    // triangle is symmetrical x-y and just a bit back from origin z
   
    const Ray ray = {
     .origin = {-1.f, 0.5f, 0.f},
     .direction = {0.f, -1.f, 0.f}
    };

    auto result = rayIntersectsVertices(ray, meshVertices);

    const Intersection expected = { 
        .point = { -1.f, 0.f, 0.f}, 
        .triangleIdx = 0 
    };

    if (result.empty()) {
        return (TestResult){
            .pass = false,
            .message = "no intersection found",
        };
    } else {
        Intersection intersection_result = result.at(0);
        if (vec3sAreEqual(expected.point, intersection_result.point) &&
            expected.triangleIdx == intersection_result.triangleIdx) {
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

TestResult intersect_vertices_last() {
   
    // triangle is symmetrical x-y and just a bit back from origin z
   
    const Ray ray = {
     .origin = { 1.f, 0.5f, 0.f},
     .direction = {0.f, -1.f, 0.f}
    };

    auto result = rayIntersectsVertices(ray, meshVertices);

    const Intersection expected = { 
        .point = { 1.f, 0.f, 0.f}, 
        .triangleIdx = 1 
    };

    if (result.empty()) {
        return (TestResult){
            .pass = false,
            .message = "no intersection found",
        };
    } else {
        Intersection intersection_result = result.at(0);
        if (vec3sAreEqual(expected.point, intersection_result.point) &&
            expected.triangleIdx == intersection_result.triangleIdx) {
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



