#include <GL/glew.h>
#include <GL/glfw.h>
// #include <SDL/SDL.h>

#include <iostream>
#include <cassert>

#include "InputManager.hpp"
#include "Entity.hpp"
// #include <SDL2/SDL.h>

using namespace std;

// TODO: Look through all the files for exceptions and handle them appropriately.

InputManager::InputManager() 
    : mPrev(0,0)
    , mCur(0,0) 
    , mUpAngle(0.0f)
    , mRightAngle(0.0f)
    , mLightStatus(false)
    , mShadowStatus(false)
    , mSubdivision(false)
    , mFractal(false)
    , mFlashlight(true) {

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

    float speed = 0.1f;

    if(glfwGetKey(GLFW_KEY_LSHIFT)) {
        speed = 0.25f;
    }

    if (glfwGetKey('S')) {
        glm::vec3 bck = -camera->Forward() * speed;
        bck = glm::vec3(bck[0], 0.0, bck[2]);

        camera->Translate(bck);
    } else if (glfwGetKey('W')) {
        glm::vec3 fwd = camera->Forward() * speed;
        fwd = glm::vec3(fwd[0], 0.0, fwd[2]);
        camera->Translate(fwd);
    }

    if (glfwGetKey('A')){
        glm::vec3 left = camera->Right() * speed;
        left = glm::vec3(left[0], 0.0, left[2]);
        camera->Translate(left);
    } else if (glfwGetKey('D')){
        glm::vec3 right = -camera->Right() * speed;
        right = glm::vec3(right[0], 0.0, right[2]);
        camera->Translate(right);
    }

    if (glfwGetKey('Z')){
        glm::vec3 up = camera->Up() * speed;
        up = glm::vec3(0.0, up[1], 0.0);
        camera->Translate(up);
    } else if (glfwGetKey('X')){
        glm::vec3 down = -camera->Up() * speed;
        down = glm::vec3(0.0, down[1], 0.0);
        camera->Translate(down);
    }

    if (glfwGetKey('2')) {
        mShadowStatus = false;
    }

    if (glfwGetKey('1')) {
        mShadowStatus = true;
    }

    if (glfwGetKey('L')) {
        mLightStatus = true;
    }
    if (glfwGetKey('K')) {
        mLightStatus = false;
    }

    if (glfwGetKey('6')) {
        mFractal = false;
    }

    if (glfwGetKey('5')) {
        mFractal = true;
    }

    if (glfwGetKey('3')) {
        mSubdivision = true;
    }

    if (glfwGetKey('4')) {
        mSubdivision = false;
    }

    if (glfwGetKey('7')) {
        mFlashlight = true;
    }

    if (glfwGetKey('8')) {
        mFlashlight = false;
    }

}

void InputManager::HandleMouseMotion(double elapsedTime) {
    int mouseX = 0, mouseY = 0;
    glfwGetMousePos(&mouseX, &mouseY);
    mouseX *= -0.1;
    mouseY *= 0.1;
    if (mouseX || mouseY) {
        auto camera = mCamera.lock();
        camera->OffsetOrientation(mouseY, mouseX);

        glfwSetMousePos(0,0);
    }

}
void InputManager::HandleMouseButton(double elapsedTime) {
    if (glfwGetMouseButton(GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
    }
}

bool InputManager::GetLightStatus() {
    return mLightStatus;
}

bool InputManager::GetShadowStatus() {
    return mShadowStatus;
}

bool InputManager::GetSubdivision() {
    return mSubdivision;
}

bool InputManager::GetFractal() {
    return mFractal;
}

bool InputManager::GetFlashlight() {
    return mFlashlight;
}

