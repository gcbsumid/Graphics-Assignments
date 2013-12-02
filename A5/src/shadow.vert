#version 330 core

layout (location = 0) in vec3 vert;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texcoord;

out VSOutput {
    invariant vec3 world_pos;
} vs_out;

uniform mat4 model_matrix;

void main() {
    // All the shader does is move everything into world space
    vs_out.world_pos = (model_matrix * vec4(vert, 1.0)).xyz;
}