#ifndef RAYCAST_H
#define RAYCAST_H

#include <vector>
#include <string>

#include "vec.h"
#include "mesh.h"
#include "scene.h"


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

std::vector<Intersection> rayIntersectsVertices(Ray ray, Vertices vertices);

std::vector<Intersection> rayIntersectsSceneNode(Ray ray, SceneNode node);

std::vector<Intersection> rayIntersectsScene(Ray ray, Scene scene);

std::vector<Intersection> sortBySceneDepth(
    std::vector<Intersection> intersections,
    Camera camera
);

#endif  