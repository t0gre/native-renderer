#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

#define STB_IMAGE_IMPLEMENTATION
#include "../third_party/stb_image.h"

#include "loaders.h"
#include "mat4.h"
#include "scene.h"
#include "material.h"



// Loads the content of a GLSL Shader file into a char* variable
char* get_shader_content(const char* fileName)
{
    long size = 0;

    /* Read File to get size */
    FILE *fp = fopen(fileName, "rb");
    if(fp == NULL) {
        printf("Fatal Error: Failed to load shader at path: %s\n", fileName);
        exit(1);
    }
    fseek(fp, 0L, SEEK_END);
    size = ftell(fp)+1;
    fclose(fp);

    /* Read File for Content */
    fp = fopen(fileName, "r");
    const auto shaderContent = static_cast<char *>(memset(malloc(size), '\0', size));
    fread(shaderContent, 1, size-1, fp);
    fclose(fp);

    return shaderContent;
}

// reads something that is not really a csv, because it has no line endings
FloatData read_csv(const char* filename) {

  FILE* fptr = fopen(filename, "r");

  int read = 0;
  int number_of_floats = 0;
  while ((read = getc(fptr)) != EOF) {
    if (read == ',') {
      number_of_floats++;
      }
    }
    
  number_of_floats++;
  const size_t number = number_of_floats;
  auto* floats = (float *)malloc(sizeof(float)*number);

  size_t float_cursor = 0;
  char number_string[10] = { 0 }; // it wont be longer than this
  size_t digit_cursor = 0;
  FILE* fptr2 = fopen(filename, "r");

  while ((read = getc(fptr2)) != EOF) {
    
    if (read != ',') {
        // add chars to number string
        const char t = read;
        number_string[digit_cursor] = t;
        digit_cursor++;
    } else {
        // convert the number string into a float
        floats[float_cursor] = atof(number_string);
        // zero the number string
        memset(number_string,0,strlen(number_string));
        // reset digit cusor
        digit_cursor = 0;
        // move onto the next float
        float_cursor++; 
    }
   } 

    // at EOF add the last float in
    floats[float_cursor] = atof(number_string);

    return (FloatData){
        .data = floats,
        .count = number
    };

}

// helper: convert Assimp matrix -> Mat4
static Mat4 mat4FromAiMatrix(const aiMatrix4x4& a) {
    Mat4 m;
    m.data[0][0] = a.a1; m.data[0][1] = a.b1; m.data[0][2] = a.c1; m.data[0][3] = a.d1;
    m.data[1][0] = a.a2; m.data[1][1] = a.b2; m.data[1][2] = a.c2; m.data[1][3] = a.d2;
    m.data[2][0] = a.a3; m.data[2][1] = a.b3; m.data[2][2] = a.c3; m.data[2][3] = a.d3;
    m.data[3][0] = a.a4; m.data[3][1] = a.b4; m.data[3][2] = a.c4; m.data[3][3] = a.d4;
    return m;
}


// Helper: load embedded texture from Assimp and decode it into TextureData
// Returns TextureData with pixel data, or empty/null data if failed
static TextureData loadEmbeddedTexture(const aiTexture* aiTex) {
    TextureData data = {};

    if (!aiTex) {
        return data;
    }

    // Decode the image using stb_image
    int width, height, channels;
    unsigned char* pixels = nullptr;
    bool needs_free = false;

    if (aiTex->mHeight == 0) {
        // Compressed format (PNG, JPEG, etc.)
        pixels = stbi_load_from_memory(
            (unsigned char*)aiTex->pcData,
            aiTex->mWidth,  // size in bytes for compressed data
            &width,
            &height,
            &channels,
            0
          );
        needs_free = true;  // stbi allocated this memory
    } else {
        // Raw RGBA data
        width = aiTex->mWidth;
        height = aiTex->mHeight;
        channels = 4;
        pixels = (unsigned char*)aiTex->pcData;
        needs_free = false;  // Assimp owns this memory
    }

    if (!pixels) {
        printf("Failed to decode texture data\n");
        return data;
    }

    // Populate TextureData
    data.pixels = pixels;
    data.width = width;
    data.height = height;
    data.channels = channels;
    data.needs_free = needs_free;
    // Default wrap modes (will be set properly in convertAiMesh based on material settings)
    data.wrapModeU = WrapMode::Wrap;
    data.wrapModeV = WrapMode::Wrap;

    return data;
}

  // Helper: convert aiMesh -> Mesh (fills Vertices.positions and Vertices.normals using DArray)
// convert a single aiMesh into our Mesh representation
Mesh convertAiMesh(const aiMesh* aMesh, const aiScene* scene) {
    Mesh m;

    m.material = BasicTextureMaterial{
        .shininess = 0.5f
    };

    const size_t vcount = aMesh->mNumVertices;
    m.vertices.vertex_count = vcount;

    // initialize index count to 0
    m.vertices.index_count = 0;

    // fill positions (3 floats per vertex)
    for (size_t i = 0; i < vcount; ++i) {
      const aiVector3D &p = aMesh->mVertices[i];
      m.vertices.positions.push_back(p.x);
      m.vertices.positions.push_back(p.y);
      m.vertices.positions.push_back(p.z);
    }

    // fill normals if present
    if (aMesh->HasNormals()) {
      for (size_t i = 0; i < vcount; ++i) {
        const aiVector3D &n = aMesh->mNormals[i];
        m.vertices.normals.push_back(n.x);
        m.vertices.normals.push_back(n.y);
        m.vertices.normals.push_back(n.z);
      }
    }

    // fill texture coordinates (uv) into the material's uvMap
    if (aMesh->HasTextureCoords(0)) {
      // Assimp supports up to 3 components per UV, but we only take u,v
      for (size_t i = 0; i < vcount; ++i) {
        const aiVector3D &uv = aMesh->mTextureCoords[0][i];
        auto &uvmap = std::get<BasicTextureMaterial>(m.material).uvMap;
        uvmap.push_back(uv.x);
        uvmap.push_back(uv.y);
      }
    }
    // fill indices from faces
    if (aMesh->mNumFaces > 0) {
      for (unsigned f = 0; f < aMesh->mNumFaces; ++f) {
        const aiFace &face = aMesh->mFaces[f];
        for (unsigned j = 0; j < face.mNumIndices; ++j) {
          m.vertices.indices.push_back(face.mIndices[j]);
        }
      }
      m.vertices.index_count = m.vertices.indices.size();
    }

    // read material texture path (diffuse) if present
    if (aMesh->mMaterialIndex >= 0 && scene && scene->mMaterials) {
      const aiMaterial* aiMat = scene->mMaterials[aMesh->mMaterialIndex];
      if (aiMat->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
        aiString texPath;
        // retrieve mapping and map modes as well
        aiTextureMapping mapping;
        unsigned int uvIndex = 0;
        float blend = 0.0f;
        aiTextureOp op = aiTextureOp_Add;
        aiTextureMapMode mapModes[2] = { aiTextureMapMode_Wrap, aiTextureMapMode_Wrap };
        if (aiMat->GetTexture(aiTextureType_DIFFUSE, 0, &texPath, &mapping, &uvIndex, &blend, &op, &mapModes[0]) == AI_SUCCESS) {
          std::string pathStr(texPath.C_Str());
          std::get<BasicTextureMaterial>(m.material).texture_path = pathStr;

          // Debug mapping info
          printf("[DEBUG] Material diffuse mapping: mapping=%d uvIndex=%u mapU=%d mapV=%d\n", (int)mapping, uvIndex, (int)mapModes[0], (int)mapModes[1]);

          // Check if it's an embedded texture (path starts with "*")
          if (pathStr.length() > 0 && pathStr[0] == '*') {
            int texIndex = std::stoi(pathStr.substr(1));
            if (texIndex >= 0 && texIndex < static_cast<int>(scene->mNumTextures)) {
              TextureData texData = loadEmbeddedTexture(scene->mTextures[texIndex]);
              if (texData.pixels != nullptr) {
                // Map Assimp wrap modes to our WrapMode enum
                auto convertWrapMode = [](aiTextureMapMode mode) -> WrapMode {
                  switch (mode) {
                    case aiTextureMapMode_Wrap:   return WrapMode::Wrap;
                    case aiTextureMapMode_Clamp:  return WrapMode::Clamp;
                    case aiTextureMapMode_Mirror: return WrapMode::Mirror;
                    case aiTextureMapMode_Decal:  return WrapMode::Decal;
                    default:                      return WrapMode::Wrap;
                  }
                };

                texData.wrapModeU = convertWrapMode(mapModes[0]);
                texData.wrapModeV = convertWrapMode(mapModes[1]);

                // Store texture data on material (GL texture will be created later by renderer)
                std::get<BasicTextureMaterial>(m.material).texture_data = texData;

                printf("Loaded embedded texture %d data (%dx%d, %d channels)\n",
                       texIndex, texData.width, texData.height, texData.channels);
              } else {
                printf("Failed to load embedded texture %d\n", texIndex);
              }
            }
          } else {
            // external textures could be handled here (not currently used for embedded glb)
          }
        }
      }
    }


    // material / id left default (populate if you have material mapping)
    m.id = std::nullopt;
    return m;
  };

  // Recursive conversion aiNode -> SceneNode (nodes allocated on heap)
SceneNode* convertNode(const aiNode* ai_node, SceneNode* parent, const aiScene * scene ) {
  
    Mat4 local = mat4FromAiMatrix(ai_node->mTransformation);
    SceneNode* node = new SceneNode(createSceneNode(local, std::nullopt, std::string(ai_node->mName.C_Str())));

    // attach to parent
    if (parent) {
      node->parent = parent;
      parent->children.push_back(node);
    }

    // If this aiNode references meshes, convert the first mesh and move it into the node.
    if (ai_node->mNumMeshes > 0 && scene->mNumMeshes > 0) {
      const aiMesh* aMesh = scene->mMeshes[ ai_node->mMeshes[0] ];
      Mesh converted = convertAiMesh(aMesh, scene);
      node->mesh.emplace(std::move(converted));
    }

    // recurse children
    for (unsigned i = 0; i < ai_node->mNumChildren; ++i) {
      convertNode(ai_node->mChildren[i], node, scene);
    }

    // update transforms for subtree
    updateWorldTransform(node);

    return node;
  };

 
SceneNode load_glb(const std::string& pFile) {
 
  // Create an instance of the Importer class
  Assimp::Importer importer;

  // And have it read the given file with some example postprocessing
  // Usually - if speed is not the most important aspect for you - you'll
  // probably to request more postprocessing than we do in this example.
  const aiScene* scene = importer.ReadFile( pFile,
    aiProcess_CalcTangentSpace       |
    aiProcess_Triangulate            |
    aiProcess_JoinIdenticalVertices  |
    aiProcess_SortByPType);

  // If the import failed, report it
  if (nullptr == scene) {

    const char* error_message = importer.GetErrorString();
    printf("%s\n", error_message);
    throw error_message;
  }
  
  // convert aiScene into SceneNode here
  SceneNode* root_ptr = convertNode(scene->mRootNode, nullptr, scene);
  // return a moved copy of the root (children remain pointers to heap nodes)
  SceneNode root = std::move(*root_ptr);
  return root;

}