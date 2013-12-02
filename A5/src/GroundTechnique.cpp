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

    mPerspectiveLocation = mProgram->Uniform("persp_matrix");
    mModelLocation = mProgram->Uniform("model_matrix");
    // mWVPLocation = mProgram->Uniform("wvp_matrix");
    return Technique::Init();
}

void GroundTechnique::SetPerspectiveMatrix(const glm::mat4& persp) {
    glUniformMatrix4fv(mPerspectiveLocation, 1, GL_TRUE, glm::value_ptr(persp));
}

void GroundTechnique::SetModelMatrix(const glm::mat4& model) {
    glUniformMatrix4fv(mModelLocation, 1, GL_TRUE, glm::value_ptr(model));
}