#version 330

layout (location = 0) in vec3 position;

uniform mat4 view_matrix;
uniform mat4 model_matrix;

out vec3 TexCoord0;

void main() {
    vec4 wvp_pos = view_matrix * model_matrix * vec4(position, 1.0);
    gl_Position = wvp_pos.xyww;
    TexCoord0 = position;
}