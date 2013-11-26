#version 330 core

in vec3 fragColour;
// in vec3 fragNormal;
// in vec2 fragCoord;

out vec4 finalColour;

// uniform struct Light {
//     vec3 position;
//     vec3 intensities;
//     float ambience_coefficient;
//     float attenuation;
// }

// uniform struct Material {
//     sampler2D tex; // This is the texture
//     float shininess;
//     vec3 specular_color;
// } material;

// uniform mat4 model_matrix;
// uniform vec3 camera_pos;


void main() {
    finalColour = vec4(fragColour.x, fragColour.y, fragColour.z, 1.0);
    // finalColour = vec4(1., 1.0);
}