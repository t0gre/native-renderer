#ifndef RAYCAST_H
#define RAYCAST_H

#include <vector>
#include <string>

#include "vec.h"
#include "mesh.h"
#include "scene.h"
#include "mystl.hpp"


typedef struct Triangle {
    Vec3 a;
    Vec3 b;
    Vec3 c;
} Triangle;

typedef struct Ray {
    Vec3 origin;
    Vec3 direction; 
} Ray;

typedef struct MeshInfo {
    Material material;
    std::optional<int> id;
} MeshInfo;


typedef struct Intersection {
    std::string nodeName; // empty if none
    Vec3 point;
    size_t triangleIdx;
    std::optional<MeshInfo> meshInfo;

} Intersection;

Vec3Result rayIntersectsTriangle(Ray ray, Triangle triangle);

DArray<Intersection> rayIntersectsVertices(Ray ray, Vertices vertices);

DArray<Intersection> rayIntersectsSceneNode(Ray ray, const SceneNode& node);

DArray<Intersection> rayIntersectsScene(const Ray &ray, const Scene& scene);

void sortBySceneDepth(
    DArray<Intersection>& intersections,
    Camera camera
);

#endif  