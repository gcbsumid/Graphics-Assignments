#version 330 core 

layout (location = 0) in vec3 vert;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texcoord;

uniform mat4 persp_matrix;
uniform mat4 model_matrix;
// uniform mat4 wvp_matrix;

out vec3 fragColour;
// out vec3 fragNormal;
// out vec2 fragCoord;

void main() {
    // fragTextCoord = texcoord;
    fragColour = vert;
    // fragNormal = (model_matrix * vec4(normal, 1.0f)).xyz;

    gl_Position = persp_matrix * model_matrix * vec4(vert, 1.0f);
    // gl_Position = model_matrix * vec4(vert, 1.0f);
}