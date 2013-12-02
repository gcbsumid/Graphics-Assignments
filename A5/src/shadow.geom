#version 330 core

layout (triangles_adjacency) in;
layout (triangle_strip, max_vertices = 18) out;

in VSOutput {
    vec3 world_pos;
} vert_in[];

uniform vec3 light_pos;
uniform mat4 persp_matrix;

float EPSILON = 0.01;

void EmitQuad(vec3 start_vert, vec3 end_vert) {
    vec3 light_dir = normalize(start_vert - light_pos);
    vec3 l = light_dir * EPSILON;
    gl_Position = persp_matrix * vec4((start_vert + l), 1.0);
    EmitVertex();

    gl_Position = persp_matrix * vec4(light_dir, 0.0);
    EmitVertex();

    light_dir = normalize(end_vert - light_pos);
    l = light_dir * EPSILON;
    gl_Position = persp_matrix * vec4((end_vert + l), 1.0);
    EmitVertex();

    gl_Position = persp_matrix * vec4(light_dir, 0.0);
    EmitVertex();

    EndPrimitive();
}

void main() {
    vec3 e1 = vert_in[2].world_pos - vert_in[0].world_pos;
    vec3 e2 = vert_in[4].world_pos - vert_in[0].world_pos;
    vec3 e3 = vert_in[1].world_pos - vert_in[0].world_pos;
    vec3 e4 = vert_in[3].world_pos - vert_in[2].world_pos;
    vec3 e5 = vert_in[4].world_pos - vert_in[2].world_pos;
    vec3 e6 = vert_in[5].world_pos - vert_in[0].world_pos;

    vec3 normal = cross(e1, e2);
    vec3 light_dir = light_pos - vert_in[0].world_pos;

    if (dot(normal, light_dir) >= 0.0) {

        normal = cross (e3, e1);
        if (dot(normal, light_dir) <= 0) {
            EmitQuad(vert_in[0].world_pos, vert_in[2].world_pos);
        }

        normal = cross(e4, e5);
        light_dir = light_pos - vert_in[2].world_pos;
        if (dot(normal, light_dir) <= 0) {
            EmitQuad(vert_in[2].world_pos, vert_in[4].world_pos);
        }

        normal = cross(e2, e6);
        light_dir = light_pos - vert_in[4].world_pos;
        if (dot(normal, light_dir) <= 0) {
            EmitQuad(vert_in[4].world_pos, vert_in[0].world_pos);
        }

        // Front cap
        light_dir = (normalize(vert_in[0].world_pos - light_pos)) * EPSILON;
        gl_Position = persp_matrix * vec4((vert_in[0].world_pos + light_dir), 1.0);
        EmitVertex();

        light_dir = (normalize(vert_in[2].world_pos - light_pos)) * EPSILON;
        gl_Position = persp_matrix * vec4((vert_in[2].world_pos + light_dir), 1.0);
        EmitVertex();

        light_dir = (normalize(vert_in[4].world_pos - light_pos)) * EPSILON;
        gl_Position = persp_matrix * vec4((vert_in[4].world_pos + light_dir), 1.0);
        EmitVertex();
        EndPrimitive();

        // Back cap
        light_dir = vert_in[4].world_pos - light_pos;
        gl_Position = persp_matrix * vec4(light_dir, 0.0);
        EmitVertex();

        light_dir = vert_in[2].world_pos - light_pos;
        gl_Position = persp_matrix * vec4(light_dir, 0.0);
        EmitVertex();

        light_dir = vert_in[0].world_pos - light_pos;
        gl_Position = persp_matrix * vec4(light_dir, 0.0);
        EmitVertex();
        EndPrimitive();
    }
}