#include "mesh.h"
#include "test_helpers.h"
#include <raycast.h>

Vertices setupData() {
    
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

    Vertices data = {
        .vertex_count = 6,
        .index_count = 0
    };


    for (size_t i = 0; i < 18; i++) {
        data.positions.push_back(pos_dat[i]);
        data.normals.push_back(norm_dat[i]);
    }

    return data;

}


TestResult intersect_vertices_first() {
   
    auto meshVertices = setupData();
    // triangle is symmetrical x-y and just a bit back from origin z
   
    const Ray ray = {
     .origin = {-1.f, 0.5f, 0.f},
     .direction = {0.f, -1.f, 0.f}
    };

    auto result = rayIntersectsVertices(ray, meshVertices);

    const VertexIntersection expected = { 
        .point = { -1.f, 0.f, 0.f}, 
        .triangleIdx = 0 
    };

    if (result.size() == 0) {
        return (TestResult){
            .pass = false,
            .message = "no intersection found",
        };
    } else {
        VertexIntersection intersection_result = result[0];
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
   
    auto meshVertices = setupData();

    // triangle is symmetrical x-y and just a bit back from origin z
   
    const Ray ray = {
     .origin = { 1.f, 0.5f, 0.f},
     .direction = {0.f, -1.f, 0.f}
    };

    auto result = rayIntersectsVertices(ray, meshVertices);

    const VertexIntersection expected = { 
        .point = { 1.f, 0.f, 0.f}, 
        .triangleIdx = 1 
    };

    if (result.size() == 0) {
        return (TestResult){
            .pass = false,
            .message = "no intersection found",
        };
    } else {
        VertexIntersection intersection_result = result[0];
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


std::vector<TestResult> runVerticesTests() {
     
    std::vector<TestResult> results;
    results.push_back(intersect_vertices_first());
    results.push_back(intersect_vertices_last());

    return results;
}