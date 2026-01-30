#include "render_program.h"
#include <loaders.h>

BasicColorRenderProgram initShader()
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

    return (BasicColorRenderProgram){
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



void drawSceneNodeBasicColor(SceneNode* node, BasicColorRenderProgram render_program) {

    if (node->mesh.has_value()) {
        

        if (std::holds_alternative<BasicColorMaterial>(node->mesh.value().material)) {

        Mesh &mesh = node->mesh.value();
        BasicColorMaterial * material = &std::get<BasicColorMaterial>(mesh.material);

        // check if the mesh has been initialized and init if not
        if (mesh.id.has_value()) {

            
            // draw this mesh
            glUseProgram(render_program.shader_program);
        
            glUniformMatrix4fv(render_program.world_matrix_uniform_location,1,0, &node->world_transform.data[0][0]);
            
            glUniform3fv(render_program.material_uniform.color_location,1, 
                material->color.data);
            glUniform3fv(render_program.material_uniform.specular_color_location,1, 
                material->specular_color.data);
            glUniform1f(render_program.material_uniform.shininess_location, 
                material->shininess);


            glBindVertexArray(mesh.id.value());
            // Draw the vertex buffer
            glDrawArrays(GL_TRIANGLES, 0, mesh.vertices.vertex_count);
        } else {
            // Initialize the mesh 
            initMesh(mesh);
            // draw mesh
            glUseProgram(render_program.shader_program);
        
            glUniformMatrix4fv(render_program.world_matrix_uniform_location,1,0, &node->world_transform.data[0][0]);
            
            glUniform3fv(render_program.material_uniform.color_location,1, 
                material->color.data);
            glUniform3fv(render_program.material_uniform.specular_color_location,1, 
                material->specular_color.data);
            glUniform1f(render_program.material_uniform.shininess_location, 
                material->shininess);


            glBindVertexArray(mesh.id.value());
            // Draw the vertex buffer
            glDrawArrays(GL_TRIANGLES, 0, node->mesh.value().vertices.vertex_count);
        }

        }
    }
    
    for (size_t i = 0; i < node->children.size(); i++) {
               drawSceneNodeBasicColor(node->children[i], render_program);
    }
}