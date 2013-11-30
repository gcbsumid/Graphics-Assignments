#include <stdexcept>
#include "NullTechnique.hpp"

using namespace std;

NullTechnique::NullTechnique() : Technique() {
    if (!AddShader("null.vert", GL_VERTEX_SHADER)) {
        throw runtime_error("Can't load null vertex shader.");
    }

    if (!AddShader("null.frag", GL_FRAGMENT_SHADER)) {
        throw runtime_error("Can't load null fragment shader.");
    }

    if (!CompileShaders()) {
        throw runtime_error("Can't compile null shader program.");
    }

    mWVPLocation = mProgram->Uniform("wvp_matrix");
}

void NullTechnique::SetWVP(const glm::mat4& wvp) {
    glUniformMatrix4fv(mWVPLocation, 1, GL_TRUE, glm::value_ptr(wvp));
}