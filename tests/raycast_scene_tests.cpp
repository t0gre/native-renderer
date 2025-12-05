#include <vector>

#include "mesh.h"
#include "scene.h"
#include "test_helpers.cpp"
#include "raycast.h"



float positions[18] = {
    -10.f, 0.f, -10.f, // back left
    -10.f, 0.f, 10.f, // front left
     10.f, 0.f, -10.f, // back right
    -10.f, 0.f, 10.f, // front left
     10.f, 0.f, 10.f, // front right
     10.f, 0.f, -10.f, // back right
     };
     
float normals[18] = {
        0.f,1.f, 0.f,
        0.f,1.f, 0.f,
        0.f,1.f, 0.f,
        0.f,1.f, 0.f,
        0.f,1.f, 0.f,
        0.f,1.f, 0.f,
};

const Vertices vertices = {
  .vertex_count = 6,
  .positions = positions,
  .normals = normals,
  
}; 
   
const Material irrelevant = {
            .color = { .r = 0.1, .g = 0.7, .b = 0.1},
            .specular_color = { .r = 0.2, .g = 0.2, .b = 0.2},
            .shininess = 0.5f
        };

TestResult intersect_node_with_position_transform() {
   

    Mat4 transform = m4fromPositionAndEuler(
            (Vec3){ .x = -2.f, .y = 0.f, .z = 0.f }, 
            (Vec3){  .x = 0.f, .y = 0.f, .z = 0.f });

    SceneNode node = initSceneNode(
            transform,
            (Mesh){
                .vertices =vertices,
                .material = irrelevant
            },
            "node"
        );
   
    const Ray ray = {
     .origin = {-11.f, 0.5f, 0.f},
     .direction = {0.f, -1.f, 0.f}
    };

    auto result = rayIntersectsSceneNode(ray, node);

    const Intersection expected = { 
        .point = { -11.f, 0.f, 0.f}, 
        .triangleIdx = 0 
    };

    if (result.size() == 0) {
        return (TestResult){
            .pass = false,
            .message = "no intersection found",
           
        };
    } else {
        Intersection intersection_result = result[0];
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

TestResult intersect_node_with_multiple_position_transform() {
   

    Mat4 transform = m4fromPositionAndEuler(
            (Vec3){ .x = -2.f, .y = 0.f, .z = 0.f }, 
            (Vec3){  .x = 0.f, .y = 0.f, .z = 0.f });

    SceneNode node = initSceneNode(
            transform,
            (Mesh){
                .vertices =vertices,
                .material = irrelevant
            },
            "node"
        );
   
   
    SceneNode parentNode = initSceneNode(
            transform,
            std::nullopt,
            "parent"
        );
   
    setParent(node, &parentNode);

     const Ray ray = {
     .origin = {-11.f, 0.5f, 0.f},
     .direction = {0.f, -1.f, 0.f}
    };


    auto result = rayIntersectsSceneNode(ray, parentNode);

    const Intersection expected = { 
        .point = { -11.f, 0.f, 0.f}, 
        .triangleIdx = 0 
    };

    if (result.size() == 0) {
        return (TestResult){
            .pass = false,
            .message = "no intersection found",
           
        };
    } else {
        Intersection intersection_result = result[0];
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

TestResult intersect_node_with_roation_transform() {
   

    Mat4 transform = m4fromPositionAndEuler(
            (Vec3){ .x = 0.f, .y = 0.f, .z = 0.f }, 
            (Vec3){  .x = 0.f, .y = 0.f, .z = PI/4.f });

    SceneNode node = initSceneNode(
            transform,
            (Mesh){
                .vertices =vertices,
                .material = irrelevant
            },
            "node"
        );
   
    const Ray ray = {
     .origin = {-1.f, 2.f, 0.f},
     .direction = {0.f, -1.f, 0.f}
    };

    auto result = rayIntersectsSceneNode(ray, node);

    const Intersection expected = { 
        .point = { -1.f, -1.f, 0.f}, 
        .triangleIdx = 0 
    };

    if (result.size() == 0) {
        return (TestResult){
            .pass = false,
            .message = "no intersection found",
           
        };
    } else {
        Intersection intersection_result = result[0];
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