#version 330 core

in vec3 fragTexCoord; // This is the texture coordinates

out vec4 finalColor;

uniform samplerCube cubemap_texture;

void main() {
    finalColor = texture(cubemap_texture, fragTexCoord);
}