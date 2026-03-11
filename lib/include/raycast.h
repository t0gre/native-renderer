#ifndef RAYCAST_H
#define RAYCAST_H

#include <string>

#include "vec.h"
#include "mesh.h"
#include "scene.h"
#include <vector>
#include "camera.h"


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

std::vector<VertexIntersection> rayIntersectsVertices(Ray ray, Vertices vertices);

std::vector<NodeIntersection> rayIntersectsSceneNode(Ray ray, const SceneNode& node);

std::vector<NodeIntersection> rayIntersectsScene(const Ray &ray, const Scene& scene);

void sortBySceneDepth(
    std::vector<NodeIntersection>& intersections,
    Camera camera
);

Vec2 getPointerClickInClipSpace(const int mouse_x, const int mouse_y, const int canvas_width, const int canvas_height);

Ray getWorldRayFromClipSpaceAndCamera(Vec2 clipSpacePoint, Camera camera);

#endif  