#include <iostream>
#include <cmath>
#include "Camera.hpp"
#include "Entity.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;

Camera::Camera(float fov,
               float near,
               float far,
               float aspect) 
    : Entity()
    , mFieldOfView(fov)
    , mNearPlane(near)
    , mFarPlane(far) 
    , mViewportAspectRatio(aspect)
    , mNeedUpdate(false)
{
}

glm::mat4 Camera::GetPerspMatrix() {
    glm::mat4 m;
    const float ar         = mViewportAspectRatio;
    const float zRange     = mNearPlane - mFarPlane;
    const double tanHalfFOV = 1 / (tan((mFieldOfView / 2.0f) * (M_PI/180.0)));

    m[0][0] = 1.0f/(tanHalfFOV * ar);
    m[1][1] = 1.0f/tanHalfFOV;
    m[2][2] = (-mNearPlane - mFarPlane)/zRange ;
    m[2][3] = 2.0f*mFarPlane*mNearPlane/zRange;
    m[3][2] = 1.0f;
    m[3][3] = 0.0;                    

    return m;

}

glm::mat4 Camera::GetGLMPerspMatrix() {
    return glm::perspective(mFieldOfView, mViewportAspectRatio, mNearPlane, mFarPlane);
}

void Camera::OffsetOrientation(float upAngle, float rightAngle) {
    mVerticalAngle += upAngle;
    mHorizontalAngle += rightAngle;

    if (mHorizontalAngle < 0.0f) {
        mHorizontalAngle += 360.0f;
    } else if (mHorizontalAngle >= 360.0f) {
        mHorizontalAngle -= 360.0f;
    }

    if (mVerticalAngle > 85.0f) {
        mVerticalAngle = 85.0f;   
    } else if (mVerticalAngle < -85.0f) {
        mVerticalAngle = -85.0f;
    }
    mRotate = glm::mat4();
    mRotate = glm::rotate(mRotate, mVerticalAngle, glm::vec3(1,0,0));
    mRotate = glm::rotate(mRotate, mHorizontalAngle, glm::vec3(0,1,0));
}

glm::mat4 Camera::GetRotate() {
    return mRotate;
}

float Camera::FieldOfView() const {
    return mFieldOfView;
}

void Camera::SetFieldOfView(const float fieldOfView) {
    mFieldOfView = fieldOfView;
    mNeedUpdate = true;
}

float Camera::NearPlane() const {
    return mNearPlane;
}

float Camera::FarPlane() const {
    return mFarPlane;
}

void Camera::SetNearAndFarPlanes(const float nearPlane, const float farPlane) {
    assert(nearPlane > 0.0f);
    assert(farPlane > nearPlane);

    mNearPlane = nearPlane;
    mFarPlane = farPlane;
    mNeedUpdate = true;
}

// void Camera::MoveCamera(float elapsedTime, glm::vec3 direction) {
//     // std::cout << "Elapsed time: " << elapsedTime << std::endl;
//     float distance = utility::MOVE_SPEED * elapsedTime;
//     glm::vec3 displacement = distance * direction;

//     // display_vec3("displacement: ", glm::vec3(displacement));
//     Translate(displacement);
// }

glm::vec3 Camera::Forward() const {
    glm::vec4 forward = glm::inverse(mRotate) * glm::vec4(0,0,-1,1);
    // forward = glm::vec4(forward[0], 0.0, forward[2], forward[3]);
    return glm::vec3(forward);
}

glm::vec3 Camera::Right() const {
    glm::vec4 right = glm::inverse(mRotate) * glm::vec4(1,0,0,1);
    // right = glm::vec4(right[0], 0.0, right[2], right[3]);
    return glm::vec3(right);
}

glm::vec3 Camera::Up() const {
    glm::vec4 up = glm::inverse(mRotate) * glm::vec4(0,1,0,1);
    // up = glm::vec4(0.0, up[1], 0.0, up[3]);
    return glm::vec3(up);
}

bool Camera::Render(Program* shader) {
    GLint cameraMatrix = shader->Uniform("camera");
    GLint cameraPosition = shader->Uniform("cameraPos");

    if (cameraMatrix == -1 || cameraPosition == -1) {
        return false;
    }

    glm::vec4 pos = glm::vec4(GetPos(), 1.0);

    glUniformMatrix4fv(cameraMatrix, 1, GL_FALSE, glm::value_ptr(mViewMatrix));
    glUniform3f(cameraPosition, pos.x, pos.y, pos.z);
    return true;
}