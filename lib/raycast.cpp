#include "raycast.h"
#include "float.h"
#include "mesh.h"
#include "vec.h"
#include "scene.h"
#include <stack>
#include <algorithm>


Ray m4RayMultiply(Ray ray, Mat4 m) {
    return (Ray){
        .origin = m4PositionMultiply(ray.origin, m),
        .direction = m4DirectionMultiply(ray.direction, m)
        
    };
}

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


std::vector<Intersection> rayIntersectsVertices(Ray ray, Vertices vertices) {
    
    std::vector<Intersection> intersections;

    float * positions = vertices.positions;

    for (size_t i = 0; i < vertices.vertex_count * 3; i += 9) {
        
        Triangle triangle = {

            {positions[i],positions[i+1],positions[i+2]}, 
            {positions[i+3],positions[i+4],positions[i+5]}, 
            {positions[i+6],positions[i+7],positions[i+8]}
        };

        Vec3Result intersectionPoint = rayIntersectsTriangle(ray, triangle);

        if (intersectionPoint.valid) {
            
           Intersection intersection = { 
            .point = intersectionPoint.value,
            .triangleIdx = i / 9
            };
           
           intersections.push_back(intersection);

        }
    }

    return intersections;
}


std::vector<Intersection> rayIntersectsSceneNode(Ray ray, SceneNode node) {
    
    std::vector<Intersection> intersections;
    std::stack<SceneNode> node_stack;
    
    
    node_stack.push(node);

    while (!node_stack.empty()) {

        SceneNode nodeUnderTest = node_stack.top();
        node_stack.pop();
        
        if (nodeUnderTest.mesh) {
            // transform the ray into mesh space
            auto inverseTransform = m4inverse(nodeUnderTest.world_transform);
            auto meshSpaceOrigin = m4PositionMultiply(
                ray.origin, 
                inverseTransform);

            auto meshSpaceDirection = m4DirectionMultiply(
                ray.direction, 
                inverseTransform);

            
            Ray newRay = {
                .origin = meshSpaceOrigin ,
                .direction = meshSpaceDirection
            }; 
            
            auto rayNodeIntersections = rayIntersectsVertices(
                newRay, 
                nodeUnderTest.mesh.value().vertices);

            if (!rayNodeIntersections.empty()) {
                for (size_t i = 0; i < rayNodeIntersections.size(); i++) {
                    auto intersection = rayNodeIntersections[i];
                    // transform the intersection back into world space
                    auto worldSpaceIntersection = m4PositionMultiply(
                        intersection.point, 
                        nodeUnderTest.world_transform);

                 
                    intersections.push_back((Intersection){ 
                        .nodeName = nodeUnderTest.name.value_or(""),
                        .point = worldSpaceIntersection, 
                        .triangleIdx = intersection.triangleIdx,
                        .meshInfo = (MeshInfo){ 
                            .material = nodeUnderTest.mesh.value().material,
                            .id = nodeUnderTest.mesh.value().id 
                        }
                    });
                }
                
            }
        }
        
        for (auto& child: nodeUnderTest.children) {
            node_stack.push(child);
        }
    }

    return intersections;
}

std::vector<Intersection> rayIntersectsScene(Ray ray, Scene scene) {
    std::vector<Intersection> intersections;
    
    for (const auto& node: scene.nodes) {
        auto rayNodeIntersections = rayIntersectsSceneNode(ray, node);
        if (!rayNodeIntersections.empty()) {
                for (auto& intersection: rayNodeIntersections) {
                     intersections.push_back(intersection);
                }
            }
        }

    return intersections;
}


std::vector<Intersection> sortBySceneDepth(
    std::vector<Intersection> intersections,
    Camera camera
) {
    auto result = intersections; // not yet sorted but we will sort in-place

    sort(result.begin(),result.end(), [camera](Intersection &a, Intersection &b){
        const auto viewMatrix = m4inverse(camera.transform);
        const auto projectionMatrix = getProjectionMatrix(camera);
        const auto viewProj = m4multiply(projectionMatrix, viewMatrix);
        const auto glPosA = m4PositionMultiply(a.point, viewProj);
        const auto glPosB = m4PositionMultiply(b.point, viewProj);

        return   glPosA.z < glPosB.z;


    });

    return result;
}