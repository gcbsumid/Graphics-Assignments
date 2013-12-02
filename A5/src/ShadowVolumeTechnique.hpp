#ifndef SHADOW_VOLUME_TECHNIQUE_HPP
#define SHADOW_VOLUME_TECHNIQUE_HPP

#include "Technique.hpp"

class ShadowVolumeTechnique : public Technique {
public:
    ShadowVolumeTechnique();

    void SetPerspectiveMatrix(const glm::mat4& cameraMat);
    void SetModelMatrix(const glm::mat4& modelMat);
    void SetLightPos(const glm::vec3& pos);

private:
    GLuint mPerspMatrixLocation;
    GLuint mModelMatrixLocation;
    GLuint mLightPositionLocation;
};

#endif