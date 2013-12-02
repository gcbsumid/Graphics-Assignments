#version 330

layout (location = 0) in vec3 vert;

out vec3 fragvert;

void main() {
    fragvert = vert;
    gl_Position = vec4(vert, 1.0);
}