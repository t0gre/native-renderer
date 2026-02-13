#include "raycast.h"
#include "float.h"
#include "mesh.h"
#include "vec.h"
#include "scene.h"
#include <stack>
#include <algorithm>
#include "mystl.hpp"


Vec3Result rayIntersectsTriangle(Ray ray, Triangle triangle) {


    const Vec3 edge1 = subtractVectors(triangle.b, triangle.a);
    const Vec3 edge2 = subtractVectors(triangle.c, triangle.a);
    const Vec3 rayCrossEdge2 = cross(ray.direction, edge2);
    const float det = dot(edge1, rayCrossEdge2);

    if (det > -FLT_EPSILON && det < FLT_EPSILON) {
        return (Vec3Result){.valid = false}; // the ray is parallel to this triangle;
    }
    
    const float invDet = 1 / det;
    const Vec3 s = subtractVectors(ray.origin, triangle.a);
    const float u = invDet * dot(s, rayCrossEdge2);

    if ((u < 0 && fabs(u) > FLT_EPSILON) || (u > 1 && fabs(u-1) > FLT_EPSILON)) {
         return (Vec3Result){.valid = false};
    }     

    const Vec3 sCrossEdge1 = cross(s, edge1);
    const float v = invDet * dot(ray.direction, sCrossEdge1);

    if ((v < 0 && fabs(v) > FLT_EPSILON) || (u + v > 1 && fabs(u + v - 1) > FLT_EPSILON)){
         return (Vec3Result){.valid = false};
    }
    // At this stage we can compute t to find out where the intersection point is on the line.
    const float t = invDet * dot(edge2, sCrossEdge1); 

     if (t > FLT_EPSILON) { // ray intersection
        return (Vec3Result){ .valid = true,
                             .value = addVectors(ray.origin, scaleVector(ray.direction, t))
                           };
    } else { // This means that there is a line intersection but not a ray intersection.
          return (Vec3Result){.valid = false};
    }
}


DArray<VertexIntersection> rayIntersectsVertices(Ray ray, Vertices vertices) {
    
    DArray<VertexIntersection> intersections;

    float * positions = vertices.positions.begin();

    for (size_t i = 0; i < vertices.vertex_count * 3; i += 9) {
        
        Triangle triangle = {

            {positions[i],positions[i+1],positions[i+2]}, 
            {positions[i+3],positions[i+4],positions[i+5]}, 
            {positions[i+6],positions[i+7],positions[i+8]}
        };

        Vec3Result intersectionPoint = rayIntersectsTriangle(ray, triangle);

        if (intersectionPoint.valid) {
            
           VertexIntersection intersection = { 
            .point = intersectionPoint.value,
            .triangleIdx = i / 9
            };
           
           intersections.push_back(intersection);

        }
    }

    return intersections;
}


DArray<NodeIntersection> rayIntersectsSceneNode(Ray ray, const SceneNode& node) {
    
    DArray<NodeIntersection> intersections;
    std::stack<const SceneNode*> node_stack;
    
    
    node_stack.push(&node);

    while (!node_stack.empty()) {

        const SceneNode * nodeUnderTest = node_stack.top();
        node_stack.pop();
        
        if (nodeUnderTest->mesh) {
            // transform the ray into mesh space
            auto inverseTransform = inverse(nodeUnderTest->world_transform);
            auto meshSpaceOrigin = positionMultiplied(
                ray.origin, 
                inverseTransform);

            auto meshSpaceDirection = directionMultiplied(
                ray.direction, 
                inverseTransform);

            
            Ray newRay = {
                .origin = meshSpaceOrigin ,
                .direction = meshSpaceDirection
            }; 
            
            auto rayNodeIntersections = rayIntersectsVertices(
                newRay, 
                nodeUnderTest->mesh.value().vertices);

            if (rayNodeIntersections.size() > 0) {
                for (const auto& intersection : rayNodeIntersections) {
                    // transform the intersection back into world space
                    auto worldSpaceIntersection = positionMultiplied(
                        intersection.point, 
                        nodeUnderTest->world_transform);

                 
                    intersections.push_back((NodeIntersection){ 
                        .id = nodeUnderTest->id,
                        .nodeName = nodeUnderTest->name.value_or(""),
                        .meshIntersection = {
                            .meshInfo = (MeshInfo){ 
                                .material = nodeUnderTest->mesh.value().material,
                                .id = nodeUnderTest->mesh.value().id
                            },
                            .vertexIntersection = {
                                .point = worldSpaceIntersection, 
                                .triangleIdx = intersection.triangleIdx,
                            }
                        }
                    });
                } 
            }
        }
        
        for (auto& child: nodeUnderTest->children) {
            node_stack.push(child);
        }
    }

    return intersections;
}

DArray<NodeIntersection> rayIntersectsScene(const Ray &ray, const Scene& scene) {
    DArray<NodeIntersection> intersections;
    
    for (const auto& node: scene.nodes) {
        auto rayNodeIntersections = rayIntersectsSceneNode(ray, *node);
        if (rayNodeIntersections.size() > 0) {
                for (const auto& intersection : rayNodeIntersections) {
                     intersections.push_back(intersection);
                }
            }
        }

    return intersections;
}


void sortBySceneDepth(
    DArray<NodeIntersection>& intersections,
    Camera camera
) {


    std::sort(intersections.begin(),intersections.end(), [camera](NodeIntersection &a, NodeIntersection &b){
        const auto viewMatrix = inverse(camera.transform);
        const auto projectionMatrix = getProjectionMatrix(camera);
        const auto viewProj = multiplied(projectionMatrix, viewMatrix);
        const auto glPosA = positionMultiplied(a.meshIntersection.vertexIntersection.point, viewProj);
        const auto glPosB = positionMultiplied(b.meshIntersection.vertexIntersection.point, viewProj);

        return   glPosA.z < glPosB.z;


    });
}