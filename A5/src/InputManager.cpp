#include <GL/glew.h>
#include <GL/glfw.h>
// #include <SDL/SDL.h>

#include <iostream>
#include <cassert>

#include "InputManager.hpp"
#include "Entity.hpp"
#include <SDL2/SDL.h>

using namespace std;

// TODO: Look through all the files for exceptions and handle them appropriately.

InputManager::InputManager() 
    : mPrev(0,0)
    , mCur(0,0) 
    , mUpAngle(0.0f)
    , mRightAngle(0.0f)
    , mLightStatus(false)
    , mShadowStatus(false) {

}

InputManager::~InputManager() {

}

void InputManager::AttachCamera(shared_ptr<Camera> comp) {
    assert(comp.use_count());
    mCamera = comp;
}

void InputManager::HandleKeyPress(double elapsedTime) {
    auto camera = mCamera.lock();
    elapsedTime *= 0.01;

    float speed = 0.05f;

    if(glfwGetKey(GLFW_KEY_LSHIFT)) {
        speed = 0.125f;
    }

    if (glfwGetKey('S')) {
        glm::vec3 bck = -camera->Forward() * speed;
        // cout << "Backward: " << bck.x << ", " << bck.y << ", " << bck.z << endl;
        bck = glm::vec3(bck[0], 0.0, bck[2]);

        camera->Translate(bck);
    } else if (glfwGetKey('W')) {
        glm::vec3 fwd = camera->Forward() * speed;
        fwd = glm::vec3(fwd[0], 0.0, fwd[2]);
        // cout << "Forward: " << fwd.x << ", " << fwd.y << ", " << fwd.z << endl;
        camera->Translate(fwd);
    }

    if (glfwGetKey('A')){
        glm::vec3 left = camera->Right() * speed;
        left = glm::vec3(left[0], 0.0, left[2]);
        // cout << "left: " << left.x << ", " << left.y << ", " << left.z << endl;
        camera->Translate(left);
    } else if (glfwGetKey('D')){
        glm::vec3 right = -camera->Right() * speed;
        right = glm::vec3(right[0], 0.0, right[2]);
        // cout << "Right: " << right.x << ", " << right.y << ", " << right.z << endl;
        camera->Translate(right);
    }

    if (glfwGetKey('Z')){
        glm::vec3 up = camera->Up() * speed;
        up = glm::vec3(0.0, up[1], 0.0);
        // cout << "Up: " << up.x << ", " << up.y << ", " << up.z << endl;
        camera->Translate(up);
    } else if (glfwGetKey('X')){
        glm::vec3 down = -camera->Up() * speed;
        down = glm::vec3(0.0, down[1], 0.0);
        // cout << "Down: " << down.x << ", " << down.y << ", " << down.z << endl;
        camera->Translate(down);
    }

    if (glfwGetKey('2')) {
        // std::cout << "Pressing L! " << std::endl;
        mLightStatus = !mLightStatus;
    }

    if (glfwGetKey('1')) {
        mShadowStatus = !mShadowStatus;
    }
}

void InputManager::HandleMouseMotion(double elapsedTime) {
    int mouseX = 0, mouseY = 0;
    glfwGetMousePos(&mouseX, &mouseY);
    // std::cout << "mUpAngle " << mUpAngle << std::endl;
    // std::cout << "mRightAngle " << mRightAngle << std::endl;
    mouseX *= -0.1;
    mouseY *= 0.1;
    if (mouseX || mouseY) {
        auto camera = mCamera.lock();
        camera->OffsetOrientation(mouseY, mouseX);

        // cameraComp->OffsetOrientation(utility::MOUSE_SENSITIVITY * mouseY, utility::MOUSE_SENSITIVITY * mouseX);
        glfwSetMousePos(0,0);
    }

}
void InputManager::HandleMouseButton(double elapsedTime) {
    if (glfwGetMouseButton(GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        cout << "Left Mouse button Pressed!" << endl;
    }
}

bool InputManager::GetLightStatus() {
    return mLightStatus;
}

bool InputManager::GetShadowStatus() {
    return mShadowStatus;
}