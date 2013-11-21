#include "SkyboxTechnique.hpp"

using namespace std;

SkyboxTechnique::SkyboxTechnique() {}

bool SkyboxTechnique::Init() {
    if (!AddShader("./skybox.vert", GL_VERTEX_SHADER)) {
        return false;
    }

    if (!AddShader("./skybox.frag", GL_FRAGMENT_SHADER)) {
        return false;
    }

    if (!CompileShaders()) {
        return false;
    }

    // mTextureLocation = mProgram->Uniform("cubemap_texture");
    return Technique::Init();
}

void SkyboxTechnique::SetTextureUnit(unsigned int TextureUnit) {
    // glUniform1i(mTextureLocation, TextureUnit);
    // mProgram->SetUniform("cubemap_texture", TextureUnit);
}
