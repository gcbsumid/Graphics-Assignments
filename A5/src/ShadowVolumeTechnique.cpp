#include <stdexcept>
#include <iostream>
#include "ShadowVolumeTechnique.hpp"

using namespace std;

ShadowVolumeTechnique::ShadowVolumeTechnique() {
    if (!AddShader("./shadow.vert", GL_VERTEX_SHADER)) {
        throw runtime_error("Can't load shadow volume vertex shader.");
    }

    if (!AddShader("./shadow.geom", GL_GEOMETRY_SHADER)) {
        throw runtime_error("Can't load shadow volume geometric shader.");
    }

    if (!AddShader("./shadow.frag", GL_FRAGMENT_SHADER)) {
        throw runtime_error("Can't load shadow volume fragment shader.");
    }

    if (!CompileShaders()) {
        throw runtime_error("Can't compile shadow volume shader.");
    }

    mPerspMatrixLocation = mProgram->Uniform("persp_matrix");
    mModelMatrixLocation = mProgram->Uniform("model_matrix");
    mLightPositionLocation = mProgram->Uniform("light_pos");
}

void ShadowVolumeTechnique::SetPerspectiveMatrix(const glm::mat4& persp) {
    glUniformMatrix4fv(mPerspMatrixLocation, 1, GL_TRUE, glm::value_ptr(persp));
}

void ShadowVolumeTechnique::SetModelMatrix(const glm::mat4& model) {
    glUniformMatrix4fv(mModelMatrixLocation, 1, GL_TRUE, glm::value_ptr(model));
}

void ShadowVolumeTechnique::SetLightPos(const glm::vec3& pos) {
    glUniform3f(mLightPositionLocation, pos.x, pos.y, pos.z);
}
