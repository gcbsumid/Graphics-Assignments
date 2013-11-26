#ifndef PIPELINE_H
#define PIPELINE_H

#include <GL/glew.h>

class Pipeline {
public:
    Pipeline();

    void SetPerspective(const glm::mat4& persp);
    void SetCamera(const glm::vec3& pos, const glm::vec3& target, const glm::vec3& up);

    const glm::mat4 GetPerspectiveCameraTrans();

    const glm::mat4 GetWVPTrans(glm::mat4 worldTransformation);

private:
    glm::mat4 CameraRotate();

    glm::mat4 mPerspMat;

    struct {
        glm::vec3 pos;
        glm::vec3 target;
        glm::vec3 up;
    } mCamera;

    glm::mat4 mWVPtransformation;
};

#endif