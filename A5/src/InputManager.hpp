#ifndef INPUTMANAGER_HPP
#define INPUTMANAGER_HPP

#include <glm/glm.hpp>
#include <memory>
#include "Component.hpp"

class InputManager {
public:
    InputManager();
    virtual ~InputManageR();

    void AttachCameraComponent(std::shared_ptr<CameraComp>);

    void HandleKeyPress(event.key);
    void HandleMouseMotiuon(event.motion);
private:
    std::weak_ptr<CameraComp> mCameraComp;
};

#endif