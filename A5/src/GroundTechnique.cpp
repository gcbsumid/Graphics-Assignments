#include "GroundTechnique.hpp"

using namespace std;

GroundTechnique::GroundTechnique() {}

bool GroundTechnique::Init() {
    // TODO: make ground.vert and ground.frag
    if (!AddShader("./ground.vert", GL_VERTEX_SHADER)) {
        return false;
    }

    if (!AddShader("./ground.frag", GL_FRAGMENT_SHADER)) {
        return false;
    }

    if (!CompileShaders()) {
        return false;
    }

    mWVPLocation = mProgram->Uniform("mvp_matrix");
    return Technique::Init();
}

void GroundTechnique::SetWVP(const glm::mat4& wvp) {
    glUniformMatrix4fv(mWVPLocation, 1, GL_TRUE, glm::value_ptr(wvp));
}