#include <chrono>
#include <string>
#include <stdexcept>
#include <iostream>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <GL/glfw.h>
#include <cmath>

#include "Engine.hpp"

using namespace std;

// TODO: Implement object creations
const glm::vec2 SCREEN_SIZE(800, 600);

Engine::Engine() {

    if (!glfwInit()) 
        throw runtime_error("glfwInit failed.");


    // open a window in GLFW
    glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 3);
    glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 3);
    glfwOpenWindowHint(GLFW_WINDOW_NO_RESIZE, GL_TRUE);
    if (!glfwOpenWindow(512, 512, 8, 8, 8, 8, 16, 0, GLFW_WINDOW))
        throw runtime_error("glfwOpenWindow failed. Hardware can't handle OpenGL 3.3");

    // initialise GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
        throw runtime_error("glewInit failed");

    // initialize GLEW
    if (!GLEW_VERSION_3_3) 
        throw runtime_error("OpenGL 3.3 Api is not available.");

    cout << "OpenGL version: " << glGetString(GL_VERSION) << endl;
    cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
    cout << "Vendor: " << glGetString(GL_VENDOR) << endl;
    cout << "Renderer: " << glGetString(GL_RENDERER) << endl;

    glfwDisable(GLFW_MOUSE_CURSOR);
    glfwSetMousePos(0,0);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glFrontFace(GL_CW);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);


    // glEnable(GL_TEXTURE_2D);
    // glEnable(GL_NORMALIZE);
    // glEnable(GL_CULL_FACE);
    // glCullFace(GL_BACK);
    // glShadeModel(GL_SMOOTH);
    // glEnable(GL_COLOR_MATERIAL);
    // glEnable(GL_DEPTH_TEST); 
    // glDepthFunc(GL_LESS);   

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
    // SDL_GL_DeleteContext(mMainContext);
    // SDL_DestroyWindow(mMainWindow);
    // SDL_Quit();
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

    // bool done = false;
    while (glfwGetWindowParam(GLFW_OPENED)) {
        currentFrame = chrono::high_resolution_clock::now();
        float tick = chrono::duration_cast<chrono::milliseconds>(currentFrame-lastFrame).count() / 1000.0f;
        Update(tick);

        // Use the frame slices
        mInput->HandleKeyPress(tick);
        mInput->HandleMouseMotion(tick);
        mInput->HandleMouseButton(tick);

        try {
            mGraphics->Render();
        } catch (exception& err) {
            cerr << "Runtime Error: " << err.what() << endl;
            break;
        }

        if (glfwGetKey(GLFW_KEY_ESC)) 
                glfwCloseWindow();
    }

    // Cleanup and exits;
    glfwTerminate();
    
    // SDL_Quit();
}

void Engine::CreateManagers() {
    mGraphics = shared_ptr<GraphicsManager>(new GraphicsManager());
    mInput = shared_ptr<InputManager>(new InputManager());
    // mResource = shared_ptr<ResourceManager>(new ResourceManager());
    mAI = shared_ptr<AIManager>(new AIManager());

    mGraphics->LinkInputManager(mInput);
    // mGraphics->LinkResourceManager(mResource);
    
    // mSound = shared_ptr<SoundManager>(new SoundManager());
    // mPhysics = shared_ptr<PhysicsManager>(new PhysicsManager());
}

void Engine::CreateObjects() {
    // Create the Camera
    mCamera = shared_ptr<Camera>(new Camera());
    mGraphics->AttachCamera(mCamera);
    mInput->AttachCamera(mCamera);

    // mCamera->Translate(glm::vec3(0.0, 10.0, 0.0));

    CreateLights();
    CreateSkybox();
    CreateSlenderman();

    // Adding all the entities
    for (auto& ent : mEntities) {
        mGraphics->AttachGameObject(ent);
    }
}

void Engine::CreateLights() {
    // Creating the Point Lights
    shared_ptr<PointLight> p1(new PointLight());

    p1->mColor = glm::vec3(1.0, 1.0, 1.0); // white
    p1->mPosition = glm::vec3(0.0, -5.0, 0.0);
    p1->mAttenuation.mLinear = 0.3f;

    mPointLights.push_back(p1);

    // Creating the Flashlight
    shared_ptr<SpotLight> s1(new SpotLight());
    s1->mColor = glm::vec3(1.0, 1.0, 1.0);
    s1->mPosition = mCamera->GetPos();
    s1->mDirection = mCamera->Forward();
    s1->mCutOff = cos(0.25 * M_PI);
    s1->mDiffuseIntensity = 0.8f;
    // s1->mAttenuation.mLinear = 0.2f;
    // s1->mAmbientIntensity = 0.2f;
    // std::cout << "Cutoff: " << s1->mCutOff <<endl;
    mSpotLights.push_back(s1);

    mGraphics->AttachSpotLights(mSpotLights);
    mGraphics->AttachPointLights(mPointLights);    
}


void Engine::CreateSkybox() {
    // Create skybox and attach to the Graphics manager
    mSkybox = shared_ptr<Skybox>(new Skybox(mCamera));
    mGraphics->AttachSkybox(mSkybox);

    mSkybox->Init(
        "resources/nightsky_right.jpg",
        "resources/nightsky_left.jpg",
        "resources/nightsky_top.jpg",
        "resources/nightsky_top.jpg",
        "resources/nightsky_front.jpg",
        "resources/nightsky_back.jpg");
    // "resources/sp3right.jpg",
        // "resources/sp3left.jpg",
        // "resources/sp3top.jpg",
        // "resources/sp3bot.jpg",
        // "resources/sp3front.jpg",
        // "resources/sp3back.jpg");

    mSkybox->Scale(glm::vec3(200.0f, 200.0f, 200.0f));
    // mSkybox->Translate(-mCamera->GetPos());

    mGround = shared_ptr<Ground>(new Ground(mCamera, 20, 20, 5, -5));
    // mGround->Translate(glm::vec3(-10,-10,-10));
    mGround->Translate(glm::vec3(0,-10,0));
    mGround->Scale(glm::vec3(10.0, 1.0, 10.0));
    mGround->AttachSpotLights(mSpotLights);
    mGround->AttachPointLights(mPointLights);
    mGraphics->AttachGround(mGround);
}

void Engine::CreateSlenderman() {
    Entity* slender = new Entity();
    if (!slender->AddMesh("resources/slenderman2.3ds")) {
        cout << "Error loading: slenderman2.3ds" << endl;
    }
    slender->Scale(glm::vec3(0.5, 0.5, 0.5));
    slender->Translate(glm::vec3(0, 0, -10));
    slender->Rotate(glm::vec3(1,0,0), -90);

    slender->AttachColor(glm::vec3(0.8,0.8,0.8), 0);
    slender->AttachColor(glm::vec3(0.8,0.8,0.8), 1);
    slender->AttachColor(glm::vec3(0.8,0.8,0.8), 2);
    slender->AttachColor(glm::vec3(0.8,0.8,0.8), 3);
    slender->AttachColor(glm::vec3(1.0,0,0), 4);
    slender->AttachColor(glm::vec3(1.0,1.0,1.0), 5);
    slender->AttachColor(glm::vec3(0,0,0), 6);
    slender->AttachColor(glm::vec3(0.8,0.8,0.8), 7);
    slender->AttachColor(glm::vec3(0,0,0), 8);

    mEntities.push_back(shared_ptr<Entity>(slender));
}