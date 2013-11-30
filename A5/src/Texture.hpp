#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <GL/glew.h>

#include <string>
#include <memory>
#include <Magick++.h>
#include "Program.hpp"

class Texture {
public:
    Texture(GLenum target, const std::string& filename);
    virtual ~Texture();

    virtual bool Load();

    virtual void Bind(std::shared_ptr<Program>& shader);
    virtual void Unbind();

    void SetDiffuse(glm::vec4 diffuse);
    void SetAmbient(glm::vec4 ambient);
    void SetSpecular(glm::vec4 specular);
    // void SetEmission (glm::vec4 emission);
    void SetShininess(float shininess);

    glm::vec4 GetDiffuse() const;
    glm::vec4 GetAmbient() const;
    glm::vec4 GetSpecular() const;
    // glm::vec4 GetEmission() const; 
    GLfloat GetShininess() const;

protected:
    std::string mFilename;
    GLenum mTextureTarget;
    GLuint mTextureObj;

    glm::vec4 mDiffuse;
    glm::vec4 mAmbient;
    glm::vec4 mSpecular;
    // glm::vec4 mEmission; // No idea what this is yet.
    GLfloat mShininess;
};

#endif