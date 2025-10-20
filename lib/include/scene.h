#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include <optional>
#include <string>

#include "light.h"
#include "vec.h"
#include "data_structures.h"
#include "camera.h"
#include "mesh.h"
#include "material.h"

typedef struct SceneNode {
    size_t id;
    Mat4 local_transform; 
    Mat4 world_transform;
    std::vector<SceneNode> children; // empty if no children
    std::optional<Mesh> mesh; 
    std::optional<SceneNode*> parent; 
    std::optional<std::string> name;
    
} SceneNode;  


void setParent(SceneNode node, SceneNode * parent);

typedef struct Scene {
    std::vector<SceneNode> nodes;
    AmbientLight ambient_light;
    DirectionalLight directional_light;
    PointLight point_light;
} Scene;

void updateWorldTransform(SceneNode * node);

void updateTransform(SceneNode * node, Mat4 transform);

SceneNode initSceneNode(Mat4 transform, std::optional<Mesh> mesh, std::string name);

#endif