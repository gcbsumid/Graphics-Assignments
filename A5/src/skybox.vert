#version 330 core

layout (location = 0) in vec3 vert;
uniform mat4 mvp_matrix;

// out vec3 TexCoord0;
// out vec3 fragVert;
out vec3 fragTexCoord;
// out vec3 fragNormal;

void main() {
    vec4 wvp_pos = mvp_matrix * vec4(vert, 1.0);
    gl_Position = wvp_pos.xyww;
    fragTexCoord = vert;
    // fragNormal = vertNormal;
    // fragVert = wvp_pos.xyz;
    // gl_Position = model_matrix * view_matrix * vec4(vert, 1.0);
    // TexCoord0 = vert;
}