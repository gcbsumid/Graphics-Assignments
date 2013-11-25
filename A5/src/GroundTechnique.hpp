#ifndef GROUND_TECHNIQUE_HPP
#define GROUND_TECHNIQUE_HPP

#include "Technique.hpp"

class GroundTechnique : public Technique {
public:
    GroundTechnique();

    virtual bool Init();

    void SetWVP(const glm::mat4& wvp);

private:

    GLuint mWVPLocation;
};

#endif