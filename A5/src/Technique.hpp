#ifndef TECHNIQUE_HPP
#define TECHNIQUE_HPP

// #include <list>
#include <memory>
#include <GL/glew.h>
#include "Shader.hpp"
#include "Program.hpp"

class Technique {
public:
    Technique();
    virtual ~Technique();

    virtual bool Init();

    void Enable();
    void Disable();

    std::shared_ptr<Program>& GetProgram();

protected:
    bool AddShader(const std::string& filePath, GLenum shaderType);

    bool CompileShaders();

    std::shared_ptr<Program> mProgram;

private:

    std::vector<Shader> mShaders;

};

#define INVALID_UNIFORM_LOCATION 0xFFFFFFFF

#endif
