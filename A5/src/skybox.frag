#version 330

in vec3 TexCoord0;

out vec4 finalColor;

// uniform samplerCube cubemap_texture;

void main() {
    // finalColor = texture(cubemap_texture, TexCoord0);
    finalColor = vec4(1, 0, 0.4, 1);
}