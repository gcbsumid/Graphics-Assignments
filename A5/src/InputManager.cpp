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
    , mLightStatus(false) {

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

    if (glfwGetKey('S')) {
        // std::cout << "Moving backwards!" << std::endl;
        // std::cout << "Pressing S! " << std::endl;
        glm::vec3 bck = -camera->Forward() * 0.05f;
        cout << "Backward: " << bck.x << ", " << bck.y << ", " << bck.z << endl;
        bck = glm::vec3(bck[0], 0.0, bck[2]);

        camera->Translate(bck);
        // cameraComp->MoveCamera(elapsedTime, -cameraComp->Forward());
    } else if (glfwGetKey('W')) {
        // std::cout << "Moving forward!" << std::endl;
        // std::cout << "Pressing W! " << std::endl;
        glm::vec3 fwd = camera->Forward() * 0.05f;
        fwd = glm::vec3(fwd[0], 0.0, fwd[2]);
        cout << "Forward: " << fwd.x << ", " << fwd.y << ", " << fwd.z << endl;
        camera->Translate(fwd);
        // cameraComp->MoveCamera(elapsedTime, cameraComp->Forward());
    }

    if (glfwGetKey('A')){
        // std::cout << "Strafing left!" << std::endl;
        // std::cout << "Pressing A! " << std::endl;

        glm::vec3 left = camera->Right() * 0.05f;
        left = glm::vec3(left[0], 0.0, left[2]);
        cout << "left: " << left.x << ", " << left.y << ", " << left.z << endl;
        camera->Translate(left);
        // cameraComp->MoveCamera(elapsedTime, -cameraComp->Right());
    } else if (glfwGetKey('D')){
        // std::cout << "Strafing right!" << std::endl;
        // std::cout << "Pressing D! " << std::endl;
        glm::vec3 right = -camera->Right() * 0.05f;
        right = glm::vec3(right[0], 0.0, right[2]);
        cout << "Right: " << right.x << ", " << right.y << ", " << right.z << endl;
        camera->Translate(right);
        // cameraComp->MoveCamera(elapsedTime, cameraComp->Right());
    }

    if (glfwGetKey('Z')){
        // std::cout << "Strafing left!" << std::endl;
        // std::cout << "Pressing A! " << std::endl;

        glm::vec3 up = camera->Up() * 0.05f;
        up = glm::vec3(0.0, up[1], 0.0);
        cout << "Up: " << up.x << ", " << up.y << ", " << up.z << endl;
        camera->Translate(up);
        // cameraComp->MoveCamera(elapsedTime, -cameraComp->Right());
    } else if (glfwGetKey('X')){
        // std::cout << "Strafing right!" << std::endl;
        // std::cout << "Pressing D! " << std::endl;
        glm::vec3 down = -camera->Up() * 0.05f;
        down = glm::vec3(0.0, down[1], 0.0);
        cout << "Down: " << down.x << ", " << down.y << ", " << down.z << endl;
        camera->Translate(down);
        // cameraComp->MoveCamera(elapsedTime, cameraComp->Right());
    }

    if (glfwGetKey('L')) {
        // std::cout << "Pressing L! " << std::endl;
        mLightStatus = !mLightStatus;
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