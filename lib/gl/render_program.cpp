#include "render_program.h"
#include "loaders.h"
#include "mesh.h"
#include "mystl.hpp"

#include <stddef.h>
#include <assert.h>
#include <stdio.h>


GLuint guaranteeUniformLocation(const GLuint program, const GLchar *name) {
    const GLuint location = glGetUniformLocation(program, name);
    assert(location != -1);
    return location;
}



void initMesh(Mesh  &mesh) {

    if (mesh.id.has_value()) {
        printf("mesh is already init, you shouldn't be trying to reinit it\n");
        
    } else {

     // sanity check
    assert(mesh.vertices.vertex_count >= 3 && "vertex_count must be >= 3");

    // setup vao
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);


    // Create vertex buffer object and copy vertex data into it
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*mesh.vertices.vertex_count*3, 
                 mesh.vertices.positions.begin(), GL_STATIC_DRAW);

    // Specify the layout of the shader vertex data (positions only, 3 floats)
    GLint posAttrib = 0;
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);

    GLuint vbo_norm;
    glGenBuffers(1, &vbo_norm);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_norm);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*mesh.vertices.vertex_count*3, 
                 mesh.vertices.normals.begin(), GL_STATIC_DRAW);

    // Specify the layout of the shader vertex data (normals only, 3 floats)
    
    GLint normAttrib = 1;
    glEnableVertexAttribArray(normAttrib);
    glVertexAttribPointer(normAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);

    if (std::holds_alternative<BasicTextureMaterial>(mesh.material)) {
        BasicTextureMaterial& texMat = std::get<BasicTextureMaterial>(mesh.material);
        if (texMat.uvMap.size() > 0) {
            GLuint vbo_uv;
            glGenBuffers(1, &vbo_uv);
            glBindBuffer(GL_ARRAY_BUFFER, vbo_uv);
            glBufferData(GL_ARRAY_BUFFER, sizeof(float) * texMat.uvMap.size(), 
                        texMat.uvMap.begin(), GL_STATIC_DRAW);

            GLint uvAttrib = 2;
            glEnableVertexAttribArray(uvAttrib);
            glVertexAttribPointer(uvAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
        }
    }

    // If we have indices, create an element array buffer bound to the VAO
    if (mesh.vertices.index_count > 0) {
        GLuint ebo;
        glGenBuffers(1, &ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * mesh.vertices.index_count,
                     mesh.vertices.indices.begin(), GL_STATIC_DRAW);
    }

    // unbind array buffer (but keep EBO bound to VAO) and VAO to avoid accidental state changes
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    mesh.id = vao;

    }

}


///////// shadows

ShadowMap createShadowMap() {
    constexpr int size = 2048;
    GLuint depthTexture;
    glGenTextures(1, &depthTexture);

    if (!depthTexture) {
         throw "failed to create depth texture";
    }
    
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, size, size, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    GLuint framebuffer;
    glGenFramebuffers(1, &framebuffer);

    if (!framebuffer) {
         throw "failed to create frame buffer";
    }

    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);

    // check completeness (helps catch errors early)
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        throw "failed to create complete framebuffer";
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return { depthTexture, framebuffer, size };
}

ShadowRenderProgram initShadowRenderProgram() {

    DArray<AttributeBinding> attribBindings;
    
    attribBindings.push_back({ .name = "a_position", .location = 0});

    const GLchar* vertexSource = get_shader_content("./shaders/depth-only.vert");
    const GLchar* fragmentSource = get_shader_content("./shaders/depth-only.frag");

    // Create and compile vertex shader
    const GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, nullptr);
    glCompileShader(vertexShader);

    // Create and compile fragment shader
    const GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, nullptr);
    glCompileShader(fragmentShader);

    // Link vertex and fragment shader into shader program and use it
    const GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    
    for (size_t i = 0; i < attribBindings.size() ; i++ )
     
    {   auto& binding = attribBindings[i];
        glBindAttribLocation(program, binding.location, binding.name);
    }
    

    glLinkProgram(program);

     if (program == -1) {
        throw "failed to create shadow render program";
    }

    return (ShadowRenderProgram){
        .program = program,
        .u_model = guaranteeUniformLocation(program, "u_model"),
        .u_lightViewProj = guaranteeUniformLocation(program, "u_lightViewProj"),
    };
}

void drawSceneNodeShadow(
    SceneNode* node,
    const ShadowRenderProgram shadowProgram,
    Mat4 lightViewProj
) {
      
   
    if (node->mesh.has_value()) {

        Mesh &mesh = node->mesh.value();
        // check if the mesh has been initialized and init if not
        if (mesh.id.has_value()) {
            // draw this mesh
            glUseProgram(shadowProgram.program);
        
            glUniformMatrix4fv(shadowProgram.u_model,1,0, &node->world_transform.data[0][0]);
            glUniformMatrix4fv(shadowProgram.u_lightViewProj,1,0, &lightViewProj.data[0][0]);

            glBindVertexArray(mesh.id.value());
            // Draw the vertex buffer using indices if available
            if (mesh.vertices.index_count > 0) {
                glDrawElements(GL_TRIANGLES, (GLsizei)mesh.vertices.index_count, GL_UNSIGNED_INT, 0);
            } else {
                glDrawArrays(GL_TRIANGLES, 0, mesh.vertices.vertex_count);
            }
        } else {
            // Initialize the mesh and store it back in the node
            initMesh(mesh);
            // draw mesh
            glUseProgram(shadowProgram.program);
        
            glUniformMatrix4fv(shadowProgram.u_model,1,0, &node->world_transform.data[0][0]);
            glUniformMatrix4fv(shadowProgram.u_lightViewProj,1,0, &lightViewProj.data[0][0]);
  
            glBindVertexArray(mesh.id.value());
            // Draw the vertex buffer using indices if available
            if (mesh.vertices.index_count > 0) {
                glDrawElements(GL_TRIANGLES, (GLsizei)mesh.vertices.index_count, GL_UNSIGNED_INT, 0);
            } else {
                glDrawArrays(GL_TRIANGLES, 0, mesh.vertices.vertex_count);
            }
        }
    }
    
    for (size_t i = 0; i < node->children.size(); i++) {
               drawSceneNodeShadow(node->children[i], shadowProgram, lightViewProj);
    }

}