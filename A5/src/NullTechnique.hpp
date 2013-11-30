#ifndef NULL_TECHNIQUE_HPP
#define NULL_TECHNIQUE_HPP

#include "Technique.hpp"

class NullTechnique : public Technique {
public:
    NullTechnique();

    virtual void SetWVP(const glm::mat4& wvp);

private:
    GLuint mWVPLocation;

};

#endif