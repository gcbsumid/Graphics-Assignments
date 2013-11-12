#ifndef ENGINE_H
#define ENGINE_H

// Standard Library
#include <memory>
#include <map>

// Own Code
// #include "AIManager.h"
// #include "InputManager.h"
// #include "GraphicsManager.h"
// #include "ResourceManager.h"
// #include "SoundManager.h"
// #include "PhysicsManager.h"
// #include "Entity.h"
// #include "Component.h"

// typedef std::map<int, Entity*> ENTITY_LIST;

class Engine {
public:
    Engine();
    ~Engine();

    // The Game Loop
    void Run();

    // Entity* GetEntity(int) const;

private:

    // This should update the dynamic objects (AI Manager)
    void Update(double);

    // This should create all my managers
    void CreateManagers();
    void CreateObjects();

    // void CreateScene();
    // void CreatePlayer();
    // void CreateLight();

    // std::unique_ptr<GraphicsManager> mGraphics;
    // std::unique_ptr<InputManager> mInput;
    // std::unique_ptr<ResourceManager> mResource;
    // std::unique_ptr<AIManager> mAI;
    // std::unique_ptr<SoundManager> mSound;
    // std::unique_ptr<PhysicsManager> mPhysics;

    // Don't implement copy constructor
    Engine(const Engine&);
    void operator=(const Engine&);

};

#endif