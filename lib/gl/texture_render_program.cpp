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


void drawSceneNodeTexture(SceneNode* node, TextureRenderProgram texture_render_program) {

    if (node->mesh.has_value()) {

        if (std::holds_alternative<BasicTextureMaterial>(node->mesh.value().material)) {

        Mesh &mesh = node->mesh.value();
        BasicTextureMaterial * material = &std::get<BasicTextureMaterial>(mesh.material);

        // Create GL texture from texture data if not yet created
        if (material->texture_id == 0 && material->texture_data.pixels != nullptr) {
            material->texture_id = createGLTextureFromData(material->texture_data);
        }

        // check if the mesh has been initialized and init if not
        if (mesh.id.has_value()) {

            // draw this mesh with texture
            glUseProgram(texture_render_program.shader_program);

            glUniformMatrix4fv(texture_render_program.world_matrix_uniform_location,1,0, &node->world_transform.data[0][0]);

            glUniform1f(texture_render_program.material_shininess_location,
                material->shininess);

            // Bind texture if loaded
            if (material->texture_id != 0) {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, material->texture_id);
                glUniform1i(texture_render_program.texture_uniform.sampler_location, 0);
            } else {
                // printf("[DEBUG] Warning: texture_id is 0, no texture will be bound\n");
            }

            glBindVertexArray(mesh.id.value());
            // Draw the vertex buffer using indices if available
            if (mesh.vertices.index_count > 0) {
                glDrawElements(GL_TRIANGLES, (GLsizei)mesh.vertices.index_count, GL_UNSIGNED_INT, 0);
            } else {
                glDrawArrays(GL_TRIANGLES, 0, mesh.vertices.vertex_count);
            }
        } else {
            // Initialize the mesh
            initMesh(mesh);

            // Create GL texture from texture data if not yet created
            if (material->texture_id == 0 && material->texture_data.pixels != nullptr) {
                material->texture_id = createGLTextureFromData(material->texture_data);
            }

            // draw mesh with texture
            glUseProgram(texture_render_program.shader_program);

            glUniformMatrix4fv(texture_render_program.world_matrix_uniform_location,1,0, &node->world_transform.data[0][0]);

            glUniform1f(texture_render_program.material_shininess_location,
                material->shininess);

            // Bind texture if loaded
            if (material->texture_id != 0) {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, material->texture_id);
                glUniform1i(texture_render_program.texture_uniform.sampler_location, 0);
            } else {
                // printf("[DEBUG] Warning: texture_id is 0 (init path), no texture will be bound\n");
            }

            glBindVertexArray(mesh.id.value());
            // Draw the vertex buffer using indices if available
            if (mesh.vertices.index_count > 0) {
                glDrawElements(GL_TRIANGLES, (GLsizei)mesh.vertices.index_count, GL_UNSIGNED_INT, 0);
            } else {
                glDrawArrays(GL_TRIANGLES, 0, mesh.vertices.vertex_count);
            }
        }

        }
    }
    
    for (size_t i = 0; i < node->children.size(); i++) {
               drawSceneNodeTexture(node->children[i], texture_render_program);
    }
}

GLuint createGLTextureFromData(const TextureData& data) {
    if (data.pixels == nullptr) {
        return 0;
    }

    // Map our WrapMode enum to GL constants
    auto toGLWrapMode = [](WrapMode mode) -> GLenum {
        switch (mode) {
            case WrapMode::Wrap:   return GL_REPEAT;
            case WrapMode::Clamp:  return GL_CLAMP_TO_EDGE;
            case WrapMode::Mirror: return GL_MIRRORED_REPEAT;
            case WrapMode::Decal:  return GL_CLAMP_TO_EDGE;  // GL_CLAMP_TO_BORDER not in GLES3
            default:               return GL_REPEAT;
        }
    };

    // Create OpenGL texture
    GLuint textureId;
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);

    // Set texture wrap modes
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, toGLWrapMode(data.wrapModeU));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, toGLWrapMode(data.wrapModeV));

    // Set texture filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Determine the correct format based on channels
    GLenum format = (data.channels == 4) ? GL_RGBA : GL_RGB;

    // Upload texture data
    glTexImage2D(GL_TEXTURE_2D, 0, format, data.width, data.height, 0, format, GL_UNSIGNED_BYTE, data.pixels);
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);

    return textureId;
}

