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
    BasicColorRenderProgram renderProgram,
    const ShadowRenderProgram shadowProgram,
    Mat4 lightViewProj
) {
      
   
    if (node->mesh.has_value()) {

        
        // check if the mesh has been initialized and init if not
        if (node->mesh.value().id.has_value()) {
            // draw this mesh
            glUseProgram(shadowProgram.program);
        
            glUniformMatrix4fv(shadowProgram.u_model,1,0, &node->world_transform.data[0][0]);
            glUniformMatrix4fv(shadowProgram.u_lightViewProj,1,0, &lightViewProj.data[0][0]);

            glBindVertexArray(node->mesh.value().id.value());
            // Draw the vertex buffer
            glDrawArrays(GL_TRIANGLES, 0, node->mesh.value().vertices.vertex_count);
        } else {
            // Initialize the mesh and store it back in the node
            node->mesh = initMesh(node->mesh.value(), &renderProgram);
            // draw mesh
            glUseProgram(shadowProgram.program);
        
            glUniformMatrix4fv(shadowProgram.u_model,1,0, &node->world_transform.data[0][0]);
            glUniformMatrix4fv(shadowProgram.u_lightViewProj,1,0, &lightViewProj.data[0][0]);
  
            glBindVertexArray(node->mesh.value().id.value());
            // Draw the vertex buffer
            glDrawArrays(GL_TRIANGLES, 0, node->mesh.value().vertices.vertex_count);
        }
    }
    
    for (size_t i = 0; i < node->children.size(); i++) {
               drawSceneNodeShadow(node->children[i], renderProgram, shadowProgram, lightViewProj);
    }

}