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

    if (glfwGetKey('S')) {
        // std::cout << "Moving backwards!" << std::endl;
        // std::cout << "Pressing S! " << std::endl;
        glm::vec3 bck = camera->Forward() * -0.2f;
        camera->Translate(bck);
        // cameraComp->MoveCamera(elapsedTime, -cameraComp->Forward());
    } else if (glfwGetKey('W')) {
        // std::cout << "Moving forward!" << std::endl;
        // std::cout << "Pressing W! " << std::endl;
        glm::vec3 fwd = camera->Forward() * 0.2f;
        camera->Translate(fwd);
        // cameraComp->MoveCamera(elapsedTime, cameraComp->Forward());
    }

    if (glfwGetKey('A')){
        // std::cout << "Strafing left!" << std::endl;
        // std::cout << "Pressing A! " << std::endl;
        glm::vec3 left = camera->Right() * -0.2f;
        camera->Translate(left);
        // cameraComp->MoveCamera(elapsedTime, -cameraComp->Right());
    } else if (glfwGetKey('D')){
        // std::cout << "Strafing right!" << std::endl;
        // std::cout << "Pressing D! " << std::endl;
        glm::vec3 right = camera->Right() * 0.2f;
        camera->Translate(right);
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
    mouseX *= 0.1;
    mouseY *= -0.1;
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




// void InputManager::HandleKeyPress(SDL_KeyboardEvent key) {
//     auto camera = mCamera.lock();
//     // TODO fix this shit. 
//     std::cout << "I'm in handle key press! " << key.keysym.sym << std::endl;
//     switch(key.keysym.sym) {
//         case SDLK_w:
//             if(key.state == SDL_PRESSED) {
//                 std::cout << "Pressing W! " << std::endl;
//                 glm::vec3 fwd = camera->Forward() * 0.2f;
//                 camera->Translate(fwd);
//                 // handle movign forward.
//             } 
//             break;
//         case SDLK_s:
//             if(key.state == SDL_PRESSED) {
//                 std::cout << "Pressing S! " << std::endl;
//                 glm::vec3 bck = camera->Forward() * -0.2f;
//                 camera->Translate(bck);
//                 // handle movign backward.
//             } 
//             break;
//         case SDLK_a:
//             if(key.state == SDL_PRESSED) {
//                 std::cout << "Pressing A! " << std::endl;
//                 glm::vec3 left = camera->Right() * -0.2f;
//                 camera->Translate(left);
//                 // handle movign strafe left.
//             } 
//             break;
//         case SDLK_d:
//             if(key.state == SDL_PRESSED) {
//                 std::cout << "Pressing D! " << std::endl;
//                 glm::vec3 right = camera->Right() * 0.2f;
//                 camera->Translate(right);
//                 // handle movign strafe right.
//             } 
//             break;
//         case SDLK_l:
//             if(key.state == SDL_PRESSED) {
//                 mLightStatus = !mLightStatus;
//             } 
//             break;
//         // TODO: deal with 12345 for different modes
//         default:
//             cout << "Default. " << endl;
//             break;
//     }
// }

// void InputManager::HandleMouseMotion(SDL_MouseMotionEvent motion) {
//     mCur[0] = (float) (motion.x - mPrev[0]) * 0.2f;
//     mCur[1] = (float) (motion.y - mPrev[0]) * 0.2f;

//     mUpAngle += mCur[1];
//     // mRightAngle += mCur[0];

//     auto camera = mCamera.lock();
//     if (mUpAngle < 75.0f && mUpAngle > -75.0f) {
//         camera->Rotate(glm::vec3(1,0,0), mCur[1]);
//     }

//     // if (mRightAngle )
//     camera->Rotate(glm::vec3(0,1,0), mCur[0]);

//     mPrev[0] = motion.x;
//     mPrev[1] = motion.y;
// }

// void InputManager::HandleMouseButton(SDL_MouseButtonEvent button) {
//     // 1 = left mouse button, 2 = middle, 3 = right
//     if (button.type == SDL_MOUSEBUTTONDOWN && button.button == 1) {
//         // TODO: pick up object ~ Probably do some raycast to intersect object
//         std::cout << "Left Click!" << std::endl;
//     } 
// }

bool InputManager::GetLightStatus() {
    return mLightStatus;
}