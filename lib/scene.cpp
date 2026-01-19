#include "scene.h"
#include "mat4.h"
#include "camera.h"



size_t sceneNodeCounter = 0;

void setParent(SceneNode& node, SceneNode& parent) {
   
   // Remove node from its current parent's children array
   if (node.parent.has_value() && node.parent.value()->id != parent.id) {
    
    SceneNode * oldParent = node.parent.value();

    for (size_t i = 0; i < oldParent->children.size(); i++) {
        
        const SceneNode * existing_child = oldParent->children[i];
        
        if (existing_child->id == node.id) {
            oldParent->children.erase(i);
        }
    }

}
    // add to the new parent
    node.parent = &parent;
    parent.children.push_back(&node); // a copy of the node
    updateWorldTransform(&parent);
}

void updateWorldTransform(SceneNode * node) {

   // n.b. this assumes the parent world transform is always up-to-date so we must keep it that way
   Mat4 parentWorldTransform;

   if (node->parent.has_value()) {
    parentWorldTransform = node->parent.value()->world_transform;
   } else {
    parentWorldTransform = m4fromPositionAndEuler({0.f,0.f,0.f}, {0.f,0.f,0.f});
   }
   
   node->world_transform = m4multiply(parentWorldTransform, node->local_transform);

   for (auto& child: node->children) {
    child->parent = node;
    updateWorldTransform(child);
   }

}

void updateTransform(SceneNode * node, const Mat4 &transform) {
   node->local_transform = transform;
   updateWorldTransform(node);
}

SceneNode initSceneNode(const Mat4 &transform, const std::optional<Mesh> &mesh, std::string name) {
   SceneNode node = {
   .id = sceneNodeCounter,
   .local_transform = transform,
   .world_transform = transform, // actually valid since there's no parent
   .children = DArray<SceneNode*>(), // empty array if no children
   .mesh = mesh,
   .name = name
};

   sceneNodeCounter++;
   updateWorldTransform(&node);
   return node;
}