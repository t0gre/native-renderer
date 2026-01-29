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

RenderProgram initShader()
{

    const GLchar* vertexSource = get_shader_content("./shaders/basic.vert");
    const GLchar* fragmentSource = get_shader_content("./shaders/basic.frag");

    // Create and compile vertex shader
    const GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, nullptr);
    glCompileShader(vertexShader);

    // Create and compile fragment shader
    const GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, nullptr);
    glCompileShader(fragmentShader);

    // Link vertex and fragment shader into shader program and use it
    const GLuint shader_program = glCreateProgram();
    glAttachShader(shader_program, vertexShader);
    glAttachShader(shader_program, fragmentShader);

     // IMPORTANT: bind attribute locations BEFORE linking so locations are consistent on WebGL2
    glBindAttribLocation(shader_program, 0, "a_position");
    glBindAttribLocation(shader_program, 1, "a_normal");

    glLinkProgram(shader_program);
    glUseProgram(shader_program);
    
    // should I ?
    // free(vertexSource);
    // free(fragmentSource);

    return (RenderProgram){
        .shader_program = shader_program,
        .world_matrix_uniform_location = guaranteeUniformLocation(shader_program, "u_model"),
        .view_uniform_location = guaranteeUniformLocation(shader_program, "u_view"),
        .projection_uniform_location = guaranteeUniformLocation(shader_program, "u_projection"),
        .view_position_uniform_location = guaranteeUniformLocation(shader_program, "u_view_position"),
        .material_uniform = {
            .color_location = guaranteeUniformLocation(shader_program, "u_material.color"),
            .specular_color_location = guaranteeUniformLocation(shader_program, "u_material.specular_color"),
            .shininess_location = guaranteeUniformLocation(shader_program, "u_material.shininess"),
        },
        .ambient_light_uniform = {
            .color_location = guaranteeUniformLocation(shader_program, "u_ambient_light.color")
        },
        .directional_light_uniform = {
            .color_location = guaranteeUniformLocation(shader_program, "u_directional_light.color"),
            .direction_location = guaranteeUniformLocation(shader_program, "u_directional_light.direction"),
        },
        .point_light_uniform = {
            .color_location = guaranteeUniformLocation(shader_program, "u_point_light.color"),
            .position_location = guaranteeUniformLocation(shader_program, "u_point_light.position"),
            .constant_location = guaranteeUniformLocation(shader_program, "u_point_light.constant"),
            .linear_location = guaranteeUniformLocation(shader_program, "u_point_light.linear"),
            .quadratic_location = guaranteeUniformLocation(shader_program, "u_point_light.quadratic")
        },
        .shadow_uniform = {
            .shadow_map_location = guaranteeUniformLocation(shader_program, "u_shadowMap"),
            .light_view_location = guaranteeUniformLocation(shader_program, "u_lightViewProj"),
        }
    }; 
}



Mesh initMesh(Mesh mesh, RenderProgram* render_program) {

    if (mesh.id.has_value()) {
        printf("mesh is already init, you shouldn't be trying to reinit it\n");
        return mesh;
    }

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

    // unbind VAO and array buffer to avoid accidental state changes
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    mesh.id = vao;

    return mesh;
}

void drawSceneNode(SceneNode* node, RenderProgram render_program) {

    if (node->mesh.has_value()) {
        
        // check if the mesh has been initialized and init if not
        if (node->mesh.value().id.has_value()) {
            // draw this mesh
            glUseProgram(render_program.shader_program);
        
            glUniformMatrix4fv(render_program.world_matrix_uniform_location,1,0, &node->world_transform.data[0][0]);
            
            glUniform3fv(render_program.material_uniform.color_location,1, 
                node->mesh.value().material.color.data);
            glUniform3fv(render_program.material_uniform.specular_color_location,1, 
                node->mesh.value().material.specular_color.data);
            glUniform1f(render_program.material_uniform.shininess_location, 
                node->mesh.value().material.shininess);


            glBindVertexArray(node->mesh.value().id.value());
            // Draw the vertex buffer
            glDrawArrays(GL_TRIANGLES, 0, node->mesh.value().vertices.vertex_count);
        } else {
            // Initialize the mesh and store it back in the node
            node->mesh = initMesh(node->mesh.value(), &render_program);
            // draw mesh
            glUseProgram(render_program.shader_program);
        
            glUniformMatrix4fv(render_program.world_matrix_uniform_location,1,0, &node->world_transform.data[0][0]);
            
            glUniform3fv(render_program.material_uniform.color_location,1, 
                node->mesh.value().material.color.data);
            glUniform3fv(render_program.material_uniform.specular_color_location,1, 
                node->mesh.value().material.specular_color.data);
            glUniform1f(render_program.material_uniform.shininess_location, 
                node->mesh.value().material.shininess);


            glBindVertexArray(node->mesh.value().id.value());
            // Draw the vertex buffer
            glDrawArrays(GL_TRIANGLES, 0, node->mesh.value().vertices.vertex_count);
        }

        
        }
    
    for (size_t i = 0; i < node->children.size(); i++) {
               drawSceneNode(node->children[i], render_program);
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
    RenderProgram renderProgram,
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