#ifndef AIMANAGER_HPP
#define AIMANAGER_HPP

#include <memory>
#include <vector>

#include "Entity.hpp"
#include "Component.hpp"
#include "Camera.hpp"

class AIManager {
public:
    AIManager();
    virtual ~AIManager();

    void GenerateAIComp(std::shared_ptr<Entity>);

    void UpdateAll();
    void Action(double);
    void AttachSlender(std::shared_ptr<Entity> slender);
    void AttachCamera(std::shared_ptr<Entity> camera);
private:
    std::vector<std::weak_ptr<AIComp>> mComponents;    
    std::shared_ptr<Entity> mSlender;
    std::shared_ptr<Camera> mCamera;
    glm::vec3 mSlenderOriginalPos;

    enum Slender_State {
        Waiting = 0,
        Searching, 
        Attacking,
        Stalking
    };

    double mTimer;
    Slender_State mState;

    int mForrestSound;
    int mEerieSound;
    int mIndicatorSound;
    int mStaticSound;

};

#endif