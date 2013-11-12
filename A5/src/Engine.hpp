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

typedef std::map<int, Entity*> ENTITY_LIST;

// TODO: Entity.cpp
// TODO: Component.cpp
// TODO: GraphicsManager.cpp
// TODO: ResourceManager.cpp
// TODO: InputManager.cpp
// TODO: AIManager.cpp
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

    // void CreateScene();
    // void CreatePlayer();
    // void CreateLight();

    std::unique_ptr<GraphicsManager> mGraphics;
    std::unique_ptr<InputManager> mInput;
    std::unique_ptr<ResourceManager> mResource;
    std::unique_ptr<AIManager> mAI;
    // std::unique_ptr<SoundManager> mSound;
    // std::unique_ptr<PhysicsManager> mPhysics;

    ENTITY_LIST mEntities;

    // Don't implement copy constructor
    Engine(const Engine&);
    void operator=(const Engine&);

};

#endif