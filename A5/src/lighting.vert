#version 330 core 

layout (location = 0) in vec3 vert;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texcoord;

uniform mat4 persp_matrix;
uniform mat4 model_matrix;
// uniform mat4 wvp_matrix;

invariant out vec3 fragVert;
invariant out vec3 fragNormal;
invariant out vec2 fragCoord;

void main() {
    gl_Position = persp_matrix * model_matrix * vec4(vert, 1.0f);
    // fragTextCoord = texcoord;
    fragVert = (model_matrix * vec4(vert, 1.0f)).xyz;
    fragNormal = normal;
    fragCoord = texcoord;

    // gl_Position = model_matrix * vec4(vert, 1.0f);
}