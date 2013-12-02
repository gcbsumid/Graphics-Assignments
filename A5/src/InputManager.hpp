#ifndef INPUTMANAGER_HPP
#define INPUTMANAGER_HPP

#include <glm/glm.hpp>
#include <memory>
#include <SDL/SDL.h>
#include "Camera.hpp"

class InputManager {
public:
    InputManager();
    virtual ~InputManager();

    void AttachCamera(std::shared_ptr<Camera>);

    void HandleKeyPress(double elapsedTime);
    void HandleMouseMotion(double elapsedTime);
    void HandleMouseButton(double elapsedTime);


    // void HandleKeyPress(SDL_KeyboardEvent key);
    // void HandleMouseMotion(SDL_MouseMotionEvent motion);
    // void HandleMouseButton(SDL_MouseButtonEvent button);

    bool GetLightStatus();
    bool GetShadowStatus();
    bool GetSubdivision(); 
    bool GetFractal(); 
    bool GetFlashlight();

private:
    std::weak_ptr<Camera> mCamera;

    glm::vec2 mPrev, mCur;
    float mUpAngle, mRightAngle;
    bool mLightStatus;
    bool mShadowStatus;     // Dropping the flashlight
    bool mSubdivision;
    bool mFractal;
    bool mFlashlight;
};

#endif