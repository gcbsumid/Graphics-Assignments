#include <stdexcept>
#include <iostream>

#include "Texture.hpp"

using namespace std;

Texture::Texture(GLenum target, const string& filename) 
    : mFilename(filename) 
    , mTextureTarget(target) {

}

Texture::~Texture() {
    glDeleteTextures(1, &mTextureObj);
}

bool Texture::Load() {
    // generating a texture name
    glGenTextures(1, &mTextureObj);
    // binding the name
    glBindTexture(mTextureTarget, mTextureObj);

    Magick::Image* image = nullptr;
    Magick::Blob blob;

    image = new Magick::Image(mFilename);

    try {
        image->write(&blob, "RGBA");
    } catch (Magick::Error& error) {
        cerr << "Error Loading texture '" << mFilename << "': " << error.what() << endl;
        delete image;
        return false;
    }

    glTexImage2D(mTextureTarget,
                    0,
                    GL_RGB,
                    image->columns(),
                    image->rows(),
                    0, 
                    GL_RGBA,
                    GL_UNSIGNED_BYTE, blob.data());

    delete image;

    // setting parameters
glTexParameteri(mTextureTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(mTextureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(mTextureTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(mTextureTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    return true;
}

void Texture::Bind(shared_ptr<Program> shader) {
    GLint tex = shader->Uniform("material.tex");
    GLint shininess = shader->Uniform("material.shininess");
    GLint specularColor = shader->Uniform("material.specular_color");

    // std::cout << "tex: " << tex << std::endl;
    // std::cout << "shininess: " << shininess << std::endl;
    // std::cout << "specularColor: " << specularColor << std::endl;

    glUniform1f(tex, 0);
    glUniform1f(shininess, mShininess);
    glUniform3f(specularColor, mSpecular[0], mSpecular[1], mSpecular[2]); // not sure if this is right though.

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(mTextureTarget, mTextureObj);
}

void Texture::Unbind() {
    glBindTexture(mTextureTarget, 0);
}

void Texture::SetDiffuse(glm::vec4 diffuse) {
    mDiffuse = diffuse;
} 

void Texture::SetAmbient(glm::vec4 ambient) {
    mAmbient = ambient;
} 

void Texture::SetSpecular(glm::vec4 specular) {
    mSpecular = specular;
} 

// void Texture::SetEmission(glm::vec4 emission) {
//     mEmission = emission;
// } 

void Texture::SetShininess(float shininess) {
    mShininess = shininess;
}

glm::vec4 Texture::GetDiffuse() const {
    return mDiffuse;
}

glm::vec4 Texture::GetAmbient() const {
    return mAmbient;
}

glm::vec4 Texture::GetSpecular() const {
    return mSpecular;
}

// glm::vec4 Texture::GetEmission() const {
//     return mEmission;
// }

GLfloat Texture::GetShininess() const {
    return mShininess;
}
