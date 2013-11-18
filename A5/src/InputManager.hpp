#ifndef INPUTMANAGER_HPP
#define INPUTMANAGER_HPP

#include <glm/glm.hpp>
#include <memory>
#include <SDL/SDL.h>
#include "Component.hpp"

class InputManager {
public:
    InputManager();
    virtual ~InputManager();

    void AttachCameraComponent(std::shared_ptr<CameraComp>);

    void HandleKeyPress(SDL_KeyboardEvent key);
    void HandleMouseMotion(SDL_MouseMotionEvent motion);
    void HandleMouseButton(SDL_MouseButtonEvent button);
private:
    std::weak_ptr<CameraComp> mCameraComp;

    glm::vec2 mPrev, mCur;
    float mUpAngle; //, mRightAngle;
};

#endif