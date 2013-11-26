#ifndef GROUND_TECHNIQUE_HPP
#define GROUND_TECHNIQUE_HPP

#include "Technique.hpp"

class GroundTechnique : public Technique {
public:
    GroundTechnique();

    virtual bool Init();

    void SetPerspectiveMatrix(const glm::mat4& persp);
    void SetModelMatrix(const glm::mat4& model);
    void SetWVPMatrix(const glm::mat4& mvp);

private:

    GLuint mWVPLocation;
    GLuint mPerspectiveLocation;
    GLuint mModelLocation;
};

#endif