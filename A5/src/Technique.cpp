#include "Technique.hpp"
#include <stdexcept>
#include <iostream>

using namespace std;

Technique::Technique()
{}

Technique::~Technique() {}

bool Technique::Init() {
    return true;
}

bool Technique::AddShader(const std::string& filepath, GLenum shaderType) {
    // std::cout << "File: " << filepath << std::endl;
    mShaders.push_back(Shader::ShaderFromFile(filepath, shaderType));
    return true;
}

bool Technique::CompileShaders() {
    try {
        mProgram = shared_ptr<Program> (new Program(mShaders));
    } catch (exception& err) {
        cerr << "Runtime Error: " << err.what() << endl;
        return false;
    }
    // mShaders.clear();
    return true;
}

shared_ptr<Program> Technique::GetProgram() {
    return mProgram;
}

void Technique::Enable() {
    if (!mProgram->IsInUse()) {
        // std::cout << "I'm being enabled!" << std::endl;
        mProgram->Use();
    }
}

void Technique::Disable() {
    if (mProgram->IsInUse()) {
        mProgram->Stop();
    }
}
