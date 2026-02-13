#ifndef RAYCAST_H
#define RAYCAST_H

#include <vector>
#include <string>

#include "vec.h"
#include "mesh.h"
#include "scene.h"
#include "mystl.hpp"


struct Triangle {
    Vec3 a;
    Vec3 b;
    Vec3 c;
};

struct Ray {
    Vec3 origin;
    Vec3 direction; 
};

struct MeshInfo {
    Material material;
    std::optional<int> id;
};

struct VertexIntersection {
    Vec3 point;
    size_t triangleIdx;
};

struct MeshIntersection {
    MeshInfo meshInfo;
    VertexIntersection vertexIntersection;
};

struct NodeIntersection {
    size_t id;
    std::string nodeName; // empty if none
    MeshIntersection meshIntersection;
};

Vec3Result rayIntersectsTriangle(Ray ray, Triangle triangle);

DArray<VertexIntersection> rayIntersectsVertices(Ray ray, Vertices vertices);

DArray<NodeIntersection> rayIntersectsSceneNode(Ray ray, const SceneNode& node);

DArray<NodeIntersection> rayIntersectsScene(const Ray &ray, const Scene& scene);

void sortBySceneDepth(
    DArray<NodeIntersection>& intersections,
    Camera camera
);

#endif  