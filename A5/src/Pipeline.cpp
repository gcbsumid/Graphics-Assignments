#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Pipeline.hpp"

using namespace std;

Pipeline::Pipeline() { }


void Pipeline::SetPerspective(const glm::mat4& persp) {
    mPerspMat = persp;
}

void Pipeline::SetCamera(const glm::vec3& pos, const glm::vec3& target, const glm::vec3& up) {
    // cout << "position: " << pos.x << ", " << pos.y << ", " << pos.z  << endl;
    // cout << "target: " << target.x << ", " << target.y << ", " << target.z  << endl;
    // cout << "up: " << up.x << ", " << up.y << ", " << up.z  << endl;
    mCamera.pos = pos;
    mCamera.target = target;
    mCamera.up = up;
}

const glm::mat4 Pipeline::GetPerspectiveCameraTrans() {
    glm::mat4 cameraTranslateTrans = glm::transpose(glm::translate(glm::mat4(), -mCamera.pos));
    glm::mat4 cameraRotateTrans = CameraRotate();
    return (cameraTranslateTrans * cameraRotateTrans * mPerspMat);
}


const glm::mat4 Pipeline::GetWVPTrans(glm::mat4 worldTransformation) {
    glm::mat4 cameraTranslateTrans = glm::transpose(glm::translate(glm::mat4(), -mCamera.pos));
    glm::mat4 cameraRotateTrans = CameraRotate();

    return (worldTransformation * cameraTranslateTrans * cameraRotateTrans * mPerspMat);
    // return (mPerspMat * cameraRotateTrans * cameraTranslateTrans * worldTransformation);
}

glm::mat4 Pipeline::CameraRotate() {
    glm::vec3 N = glm::normalize(mCamera.target);
    glm::vec3 U = glm::normalize(mCamera.up);
    U = glm::cross(U,N);
    glm::vec3 V = glm::cross(N,U);

    glm::mat4 m;
    m[0][0] = U.x;   m[0][1] = U.y;   m[0][2] = U.z;   m[0][3] = 0.0f;
    m[1][0] = V.x;   m[1][1] = V.y;   m[1][2] = V.z;   m[1][3] = 0.0f;
    m[2][0] = N.x;   m[2][1] = N.y;   m[2][2] = N.z;   m[2][3] = 0.0f;
    m[3][0] = 0.0f;  m[3][1] = 0.0f;  m[3][2] = 0.0f;  m[3][3] = 1.0f; 

    return m;  
}