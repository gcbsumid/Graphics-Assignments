#version 330 core

layout (location = 0) in vec3 vert;
// layout (location = 1) in vec3 normal;
// layout (location = 2) in vec2 texcoord;

uniform mat4 mvp_matrix;

out vec3 fragTexCoord;

void main() {
    vec4 wvp_pos = mvp_matrix * vec4(vert, 1.0);
    gl_Position = wvp_pos.xyww;
    fragTexCoord = vert;
}