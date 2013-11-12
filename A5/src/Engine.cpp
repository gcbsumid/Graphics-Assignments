#include <chrono>
#include <string>
#include <stdexcept>
#include <iostream>

#include <GL/glew.h>
#include <GL/glfw.h>
#include <glm/glm.hpp>

#include "Engine.hpp"

using namespace std;

// TODO: Implement managers
// TODO: Implement object creations
const glm::vec2 SCREEN_SIZE(800, 600);

Engine::Engine() {
    // Initialize GLFW
    if (!glfwInit()) {
        throw runtime_error("glfwInit failed.");
    }

    glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 3);
    glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 3);
    glfwOpenWindowHint(GLFW_WINDOW_NO_RESIZE, GL_TRUE);
    if (!glfwOpenWindow(SCREEN_SIZE.x, SCREEN_SIZE.y, 8, 8, 8, 8, 16, 0, GLFW_WINDOW)) {
        throw runtime_error("glfwOpenWindow failed. Hardware can't handle OpenGL 3.3");
    }
    glfwSetWindowTitle("Slender Boy");

    // initialise GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        throw runtime_error("glewInit failed");
    }

    cout << "OpenGL version: " << glGetString(GL_VERSION) << endl;
    cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
    cout << "Vendor: " << glGetString(GL_VENDOR) << endl;
    cout << "Renderer: " << glGetString(GL_RENDERER) << endl;

    // GLFW settings
    // glfwDisable(GLFW_MOUSE_CURSOR);
    // glfwSetMousePos(0,0);
    // glfwSetMouseWheel(0);
    
    if (!GLEW_VERSION_3_3) 
        throw runtime_error("OpenGL 3.3 Api is not available.");

    // TODO: Init GL_DEPTH_TEST, glDepthFunc(GL_LESS)

    CreateManagers();
    CreateObjects();
}

Engine::~Engine() {
    // Todo: Delete Entities;
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

    while (glfwGetWindowParam(GLFW_OPENED)) {
        auto currentFrame = chrono::high_resolution_clock::now();

        Update(chrono::duration_cast<chrono::milliseconds>(currentFrame-lastFrame).count() / 1000.0f);

        // mGraphics->Render();
        if (glfwGetKey('4')==GLFW_PRESS) {
            cerr << "bitches." << endl;
        }

        if(glfwGetKey(GLFW_KEY_ESC) ==GLFW_PRESS ) {
            glfwCloseWindow();
        }

        lastFrame = currentFrame;
    }

    // Cleanup and exits;
    glfwTerminate();
}

void Engine::CreateManagers() {
    // TODO: Create Managers;

    // mGraphics = unique_ptr<GraphicsManager>(new GraphicsManager());
    // mInput = unique_ptr<InputManager>(new InputManager());
    // mResource = unique_ptr<ResourceManager>(new ResourceManager());
    // mAI = unique_ptr<AIManager>(new AIManager());
    // mSound = unique_ptr<SoundManager>(new SoundManager());
    // mPhysics = unique_ptr<PhysicsManager>(new PhysicsManager());
}

void Engine::CreateObjects() {
    // TODO: Create Managers;

    // mGraphics = unique_ptr<GraphicsManager>(new GraphicsManager());
    // mInput = unique_ptr<InputManager>(new InputManager());
    // mResource = unique_ptr<ResourceManager>(new ResourceManager());
    // mAI = unique_ptr<AIManager>(new AIManager());
    // mSound = unique_ptr<SoundManager>(new SoundManager());
    // mPhysics = unique_ptr<PhysicsManager>(new PhysicsManager());
}