#ifndef SCENE_H
#define SCENE_H

#include <optional>
#include <string>

#include "light.h"
#include "camera.h"
#include "mesh.h"
#include "mystl.hpp"

typedef struct SceneNode {
    size_t id;
    Mat4 local_transform; 
    Mat4 world_transform;
    DArray<SceneNode *> children; // empty if no children
    std::optional<Mesh> mesh; 
    std::optional<SceneNode *> parent;
    std::optional<std::string> name;
    
} SceneNode;  


void setParent(SceneNode& node, SceneNode& parent);

typedef struct Scene {
    DArray<SceneNode*> nodes;
    AmbientLight ambient_light;
    DirectionalLight directional_light;
    PointLight point_light;
} Scene;

void updateWorldTransform(SceneNode * node);

void updateTransform(SceneNode * node, const Mat4 &transform);

SceneNode createSceneNode(const Mat4 &transform, const std::optional<Mesh> &mesh, std::string name);

#endif