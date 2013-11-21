#include "Technique.hpp"
#include <iostream>

using namespace std;

Technique::Technique()
{}

Technique::~Technique() {}

bool Technique::Init() {
    mViewMatrixLocation = mProgram->Uniform("view_matrix");
    mModelMatrixLocation = mProgram->Uniform("model_matrix");
    return true;
}

void Technique::SetViewMatrix(const glm::mat4& viewMat) {
    mProgram->SetUniform("view_matrix", viewMat, GL_TRUE);
}

void Technique::SetModelMatrix(const glm::mat4& modelMat) {
    mProgram->SetUniform("model_matrix", modelMat, GL_TRUE);
}

bool Technique::AddShader(const std::string& filepath, GLenum shaderType) {
    std::cout << "File: " << filepath << std::endl;
    mShaders.push_back(Shader::ShaderFromFile(filepath, shaderType));
    return true;
}

bool Technique::CompileShaders() {
    mProgram = shared_ptr<Program> (new Program(mShaders));
    // mShaders.clear();
    return true;
}

shared_ptr<Program> Technique::GetProgram() {
    return mProgram;
}

void Technique::Enable() {
    if (!mProgram->IsInUse()) {
        std::cout << "I'm being enabled!" << std::endl;
        mProgram->Use();
    }
}

void Technique::Disable() {
    if (mProgram->IsInUse()) {
        mProgram->Stop();
    }
}
