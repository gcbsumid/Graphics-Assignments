#version 330 core 

layout (location = 0) in vec3 vert;
invariant gl_Position;
uniform mat4 wvp_matrix;

void main() {
    gl_Position = wvp_matrix * vec4(vert, 1.0);
}
