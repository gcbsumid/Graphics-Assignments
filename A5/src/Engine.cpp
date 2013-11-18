#include <chrono>
#include <string>
#include <stdexcept>
#include <iostream>

#include <glm/glm.hpp>

#include "Engine.hpp"
#include "SDL_OGL.h" 

using namespace std;

// TODO: Implement object creations
const glm::vec2 SCREEN_SIZE(800, 600);

Engine::Engine() {

    if (CreateGLWindow("Slender Boy", SCREEN_SIZE.x, SCREEN_SIZE.y, 16, 0) == 0) {
        KillGLWindow();
        throw runtime_error("Could not initialize OpenGL\n\n");
    }

    // Hide the mouse cursor 
    // SDL_ShowCursor(0);

    CreateManagers();
    CreateObjects();
}

Engine::~Engine() {
    // Todo: Delete Entities;
    // for (auto& ent : mEntities) {
    //     delete ent.second;
    // }
}

void Engine::Update(double timeTick) {
    // deal with input
    // mInput->HandleInput(timeTick);

    // Deal with Physics
    // mPhysics->HandleInput(timeTick);

    // Deal with AI
    // mAI->Action(timeTick);    
}

void Engine::Run() {
    auto lastFrame = chrono::high_resolution_clock::now();
    auto currentFrame = lastFrame;

    Uint8* keys; 
    bool done = false;
    while (!done) {
        currentFrame = chrono::high_resolution_clock::now();
        Update(chrono::duration_cast<chrono::milliseconds>(currentFrame-lastFrame).count() / 1000.0f);

        try {
            mGraphics->Render();
        } catch (exception& err) {
            cerr << "Runtime Error: " << err.what() << endl;
            break;
        }


        SDL_Event event;
        if (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_KEYDOWN:
                case SDL_KEYUP:
                    mInput->HandleKeyPress(event.key);
                    break;
                case SDL_MOUSEMOTION:
                    SDL_PeepEvents(&event, 9, SDL_GETEVENT, SDL_MOUSEMOTION);
                    mInput->HandleMouseMotion(event.motion);
                    break;
                case SDL_MOUSEBUTTONDOWN:
                case SDL_MOUSEBUTTONUP:
                    mInput->HandleMouseButton(event.button);
                case SDL_QUIT:
                    done = true;
                    break;
                default:
                    break;
            }

            keys = SDL_GetKeyState(NULL);
            if (keys[SDLK_ESCAPE]) {
                break;
            }
        }
        lastFrame = currentFrame;
    }

    // Cleanup and exits;
    KillGLWindow();
}

void Engine::CreateManagers() {
    mGraphics = shared_ptr<GraphicsManager>(new GraphicsManager());
    mInput = shared_ptr<InputManager>(new InputManager());
    mResource = shared_ptr<ResourceManager>(new ResourceManager());
    mAI = shared_ptr<AIManager>(new AIManager());

    mGraphics->LinkInputManager(mInput);
    mGraphics->LinkResourceManager(mResource);
    
    // mSound = shared_ptr<SoundManager>(new SoundManager());
    // mPhysics = shared_ptr<PhysicsManager>(new PhysicsManager());
}

void Engine::CreateObjects() {

    CreateSkybox();
}

void Engine::CreateSkybox() {
    Entity* skybox = new Entity();
    mEntities["skybox"] = shared_ptr<Entity>(skybox);
    mGraphics->GenerateDrawComp(mEntities.at("skybox"));

        // TODO: SKYBOX
}