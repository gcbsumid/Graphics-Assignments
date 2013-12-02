#include <iostream>
#include "CubemapTexture.hpp"

using namespace std;

const GLenum types[6] = {
    GL_TEXTURE_CUBE_MAP_POSITIVE_X,
    GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
    GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
    GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
    GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
    GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
};

CubemapTexture::CubemapTexture(const std::string& PosXFilename,
               const std::string& NegXFilename,
               const std::string& PosYFilename,
               const std::string& NegYFilename,
               const std::string& PosZFilename,
               const std::string& NegZFilename) 
    : Texture(GL_TEXTURE_CUBE_MAP, "CUBEMAP") {
    mFilenames[0] = PosXFilename;
    mFilenames[1] = NegXFilename;
    mFilenames[2] = PosYFilename;
    mFilenames[3] = NegYFilename;
    mFilenames[4] = PosZFilename;
    mFilenames[5] = NegZFilename;
}

CubemapTexture::~CubemapTexture() {
    if (mTextureObj != 0) {
        glDeleteTextures(1, &mTextureObj);
    }
}

bool CubemapTexture::Load() {
    
    glGenTextures(1, &mTextureObj);
    glBindTexture(GL_TEXTURE_CUBE_MAP, mTextureObj);

    Magick::Image* image = NULL;
    Magick::Blob blob;

    for (unsigned int i = 0 ; i < 6 ; i++) {
        image = new Magick::Image(mFilenames[i]);

        try { 
            image->write(&blob, "RGBA");
        }
        catch (Magick::Error& Error) {
            cerr << "Error loading texture '" << mFilenames[i] << "': " << Error.what() << endl;
            delete image;
            return false;
        }
        glTexImage2D(types[i], 0, GL_RGB, image->columns(), image->rows(), 0, GL_RGBA,
            GL_UNSIGNED_BYTE, blob.data());

        delete image;
    } 

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return true;
}

void CubemapTexture::Bind(shared_ptr<Program> shader) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, mTextureObj);
}

void CubemapTexture::Unbind() {
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}
