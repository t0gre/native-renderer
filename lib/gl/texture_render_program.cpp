#include "render_program.h"
#include <loaders.h>

TextureRenderProgram initTextureShader()
{

    const GLchar* vertexSource = get_shader_content("./shaders/basic-texture.vert");
    const GLchar* fragmentSource = get_shader_content("./shaders/basic-texture.frag");

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
    glBindAttribLocation(shader_program, 2, "a_texcoord");

    glLinkProgram(shader_program);
    glUseProgram(shader_program);

    return (TextureRenderProgram){
        .shader_program = shader_program,
        .world_matrix_uniform_location = guaranteeUniformLocation(shader_program, "u_model"),
        .view_uniform_location = guaranteeUniformLocation(shader_program, "u_view"),
        .projection_uniform_location = guaranteeUniformLocation(shader_program, "u_projection"),
        .view_position_uniform_location = guaranteeUniformLocation(shader_program, "u_view_position"),
        .material_shininess_location = guaranteeUniformLocation(shader_program, "u_material.shininess"),
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
        },
        .texture_uniform = {
            .sampler_location = guaranteeUniformLocation(shader_program, "mesh_texture")
        }
    }; 
}


Mesh initTextureMesh(Mesh mesh, TextureRenderProgram* render_program) {

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

    // Set up UV coordinates VBO
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

    // unbind VAO and array buffer to avoid accidental state changes
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    mesh.id = vao;

    return mesh;
}


void drawSceneNodeTexture(SceneNode* node, TextureRenderProgram render_program) {

    if (node->mesh.has_value()) {

        if (std::holds_alternative<BasicTextureMaterial>(node->mesh.value().material)) {

        Mesh * mesh = &node->mesh.value();
        BasicTextureMaterial * material = &std::get<BasicTextureMaterial>(mesh->material);

        // check if the mesh has been initialized and init if not
        if (mesh->id.has_value()) {

            // draw this mesh with texture
            glUseProgram(render_program.shader_program);
        
            glUniformMatrix4fv(render_program.world_matrix_uniform_location,1,0, &node->world_transform.data[0][0]);
            
            glUniform1f(render_program.material_shininess_location, 
                material->shininess);

            // Bind texture if loaded
            if (material->texture_id != 0) {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, material->texture_id);
                glUniform1i(render_program.texture_uniform.sampler_location, 0);
            }

            glBindVertexArray(mesh->id.value());
            // Draw the vertex buffer
            glDrawArrays(GL_TRIANGLES, 0, mesh->vertices.vertex_count);
        } else {
            // Initialize the mesh and store it back in the node
            node->mesh = initTextureMesh(*mesh, &render_program);
            // draw mesh with texture
            glUseProgram(render_program.shader_program);
        
            glUniformMatrix4fv(render_program.world_matrix_uniform_location,1,0, &node->world_transform.data[0][0]);
            
            glUniform1f(render_program.material_shininess_location, 
                material->shininess);

            // Bind texture if loaded
            if (material->texture_id != 0) {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, material->texture_id);
                glUniform1i(render_program.texture_uniform.sampler_location, 0);
            }

            glBindVertexArray(mesh->id.value());
            // Draw the vertex buffer
            glDrawArrays(GL_TRIANGLES, 0, mesh->vertices.vertex_count);
        }

        }
    }
    
    for (size_t i = 0; i < node->children.size(); i++) {
               drawSceneNodeTexture(node->children[i], render_program);
    }
}

