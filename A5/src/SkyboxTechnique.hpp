#ifndef SKYBOX_TECHNIQUE_HPP
#define SKYBOX_TECHNIQUE_HPP

#include "Technique.hpp"

class SkyboxTechnique : public Technique {
public:
    SkyboxTechnique();

    virtual bool Init();

    void SetTextureUnit(unsigned int TextureUnit);
    void SetWVP(const glm::mat4& wvp);

private:
    GLuint mWVPLocation;
    GLuint mTextureLocation;  
};

#endif