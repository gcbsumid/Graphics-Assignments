#ifndef ENGINE_HPP
#define ENGINE_HPP

// Standard Library
#include <memory>
#include <map>

// Own Code
#include "AIManager.hpp"
#include "InputManager.hpp"
#include "GraphicsManager.hpp"
#include "ResourceManager.hpp"
// #include "SoundManager.hpp"
// #include "PhysicsManager.hpp"
#include "Entity.hpp"
#include "Component.hpp"

typedef std::map<std::string, std::shared_ptr<Entity>> ENTITY_LIST;

// TODO: SoundManager.hpp/cpp
// TODO: PhysicsManager.hpp/cpp


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

    void CreateSkybox(); 
    // void CreateScene();
    // void CreatePlayer();
    // void CreateLight();

    std::shared_ptr<GraphicsManager> mGraphics;
    std::shared_ptr<InputManager> mInput;
    std::shared_ptr<ResourceManager> mResource;
    std::shared_ptr<AIManager> mAI;
    // std::unique_ptr<SoundManager> mSound;
    // std::unique_ptr<PhysicsManager> mPhysics;

    ENTITY_LIST mEntities;

    // Don't implement copy constructor
    Engine(const Engine&);
    void operator=(const Engine&);

};

#endif