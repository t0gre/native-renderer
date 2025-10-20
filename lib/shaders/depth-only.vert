#version 300 es
precision highp float;

layout(location = 0) in vec3 a_position;
uniform mat4 u_model;
uniform mat4 u_lightViewProj;
void main() {
    gl_Position = u_lightViewProj * u_model * vec4(a_position, 1.0);
}