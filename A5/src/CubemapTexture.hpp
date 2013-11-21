#ifndef CUBEMAPTEXTURE_HPP
#define CUBEMAPTEXTURE_HPP

#include <string>
#include <memory>
#include "Texture.hpp"

class CubemapTexture : public Texture {
public:
    CubemapTexture(const std::string& PosXFilename,
                   const std::string& NegXFilename,
                   const std::string& PosYFilename,
                   const std::string& NegYFilename,
                   const std::string& PosZFilename,
                   const std::string& NegZFilename);

    virtual ~CubemapTexture();

    virtual bool Load();
    virtual void Bind(std::shared_ptr<Program> shader);
    virtual void Unbind();

protected:
    std::string mFilenames[6];
};

#endif