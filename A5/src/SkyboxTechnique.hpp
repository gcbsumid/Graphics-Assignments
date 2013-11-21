#ifndef SKYBOX_TECHNIQUE_HPP
#define SKYBOX_TECHNIQUE_HPP

#include "Technique.hpp"

class SkyboxTechnique : public Technique {
public:
    SkyboxTechnique();

    virtual bool Init();

    void SetTextureUnit(unsigned int TextureUnit);

private:
    // GLuint mWVPLocation;
    GLuint mTextureLocation;  
};

#endif