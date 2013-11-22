#version 330 core

// in vec3 fragNormal;
in vec3 fragTexCoord; // This is the texture coordinates
// in vec3 fragVert; 

out vec4 finalColor;

uniform samplerCube cubemap_texture;

void main() {
    finalColor = texture(cubemap_texture, fragTexCoord);
    // finalColor = vec4(0.4f, 0.4f, 0.4f, 1.0f);
}