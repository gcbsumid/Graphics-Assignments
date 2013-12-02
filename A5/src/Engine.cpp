#include <chrono>
#include <string>
#include <stdexcept>
#include <iostream>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <GL/glfw.h>
#include <cmath>
#include <random>
#include <glm/gtc/matrix_transform.hpp>

#include "Engine.hpp"

using namespace std;

// TODO: Implement object creations
const glm::vec2 SCREEN_SIZE(1900, 1200);

Engine::Engine() {

    mSubdivision = false;
    mFractal = false;

    if (!glfwInit()) 
        throw runtime_error("glfwInit failed.");

    // open a window in GLFW
    glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 3);
    glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 3);
    glfwOpenWindowHint(GLFW_WINDOW_NO_RESIZE, GL_TRUE);
    if (!glfwOpenWindow(SCREEN_SIZE.x, SCREEN_SIZE.y, 8, 8, 8, 8, 32, 24, GLFW_WINDOW))
        throw runtime_error("glfwOpenWindow failed. Hardware can't handle OpenGL 3.3");

    // initialise GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
        throw runtime_error("glewInit failed");

    // initialize GLEW
    if (!GLEW_VERSION_3_3) 
        throw runtime_error("OpenGL 3.3 Api is not available.");

    // cout << "OpenGL version: " << glGetString(GL_VERSION) << endl;
    // cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
    // cout << "Vendor: " << glGetString(GL_VENDOR) << endl;
    // cout << "Renderer: " << glGetString(GL_RENDERER) << endl;

    glfwDisable(GLFW_MOUSE_CURSOR);
    glfwSetMousePos(0,0);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glFrontFace(GL_CW);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_DEPTH_CLAMP);
    glEnable(GL_TEXTURE_2D);
    glDepthFunc(GL_LEQUAL);
    glDepthRange(0.1, 1.0);

    CreateManagers();
    CreateObjects();
}

Engine::~Engine() {
}

void Engine::Update(double timeTick) {
    mAI->Action(timeTick);
}

void Engine::Run() {
    auto lastFrame = chrono::high_resolution_clock::now();
    auto currentFrame = lastFrame;
    double last_time = 0;

    mAI->StartPlayingSounds();

    while (glfwGetWindowParam(GLFW_OPENED)) {
        currentFrame = chrono::high_resolution_clock::now();
        float cur_time = chrono::duration_cast<chrono::milliseconds>(currentFrame-lastFrame).count() / 1000.0f;
        double tick = (double)cur_time - last_time;
        last_time = cur_time;
        Update(tick);

        // Use the frame slices
        mInput->HandleKeyPress(tick);
        mInput->HandleMouseMotion(tick);
        mInput->HandleMouseButton(tick);

        if (mInput->GetSubdivision() != mSubdivision || mInput->GetFractal() != mFractal) {
            mSubdivision = mInput->GetSubdivision();
            mFractal = mInput->GetFractal();
            mGround->Rebuild(mSubdivision, mFractal);
        }

        try {
            if (mAI->IsBlurred()) {
                mGraphics->DisplayStatic();
            } else {
                mGraphics->Render();
            }
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
    mAI = shared_ptr<AIManager>(new AIManager());

    mGraphics->LinkInputManager(mInput);
}

void Engine::CreateObjects() {
    // Create the Camera
    mCamera = shared_ptr<Camera>(new Camera(90.0f, 0.1f, 100.0f, (float)SCREEN_SIZE.x/(float)SCREEN_SIZE.y));
    mGraphics->AttachCamera(mCamera);
    mInput->AttachCamera(mCamera);
    mAI->AttachCamera(mCamera);

    mCamera->Translate(glm::vec3(0.0, 2.0, 0.0));

    CreateSlenderman();
    CreateLights();
    CreateSkybox();

    // Adding all the entities
    for (auto& ent : mEntities) {
        mGraphics->AttachGameObject(ent);
    }
}

void Engine::CreateLights() {
    // Creating the Point Lights
    shared_ptr<PointLight> p1(new PointLight());

    p1->mColor = glm::vec3(1.0, 1.0, 1.0); // white
    p1->mPosition = glm::vec3(mCamera->GetPos());
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

    mSkybox->Scale(glm::vec3(200.0f, 200.0f, 200.0f));

    mGround = shared_ptr<Ground>(new Ground(mCamera, 20, 20, 5, -5));
    mGround->Scale(glm::vec3(10.0, 1.0, 10.0));
    mGround->AttachSpotLights(mSpotLights);
    mGround->AttachPointLights(mPointLights);
    mGraphics->AttachGround(mGround);
}

void Engine::CreateSlenderman() {
    shared_ptr<Entity> slender(new Entity());
    if (!slender->AddMesh("resources/slenderman.3ds", true)) {
        cerr << "Error loading: slenderman.3ds" << endl;
    }
    slender->Scale(glm::vec3(0.5, 0.5, 0.5));
    slender->Translate(glm::vec3(-10, 1.4, -2));

    slender->AttachColor(glm::vec3(0.8f,0.8f,0.8f), 0);
    slender->AttachColor(glm::vec3(0.8f,0.8f,0.8f), 1);
    slender->AttachColor(glm::vec3(0.8f,0.8f,0.8f), 2);
    slender->AttachColor(glm::vec3(0.8f,0.8f,0.8f), 3);
    slender->AttachColor(glm::vec3(1.0f,0.0f,0.0f), 4);
    slender->AttachColor(glm::vec3(1.0f,1.0f,1.0f), 5);
    slender->AttachColor(glm::vec3(0.05f,0.05f,0.05f), 6);
    slender->AttachColor(glm::vec3(0.8f,0.8f,0.8f), 7);
    slender->AttachColor(glm::vec3(0.05f,0.05f,0.05f), 8);

    mAI->AttachSlender(slender);

    mEntities.push_back(slender);

    Entity* crate = new Entity();
    if (!crate->AddMesh("resources/Crate1.obj", true)) {
        cerr << "Error loading: crate.3ds" << endl;
    }

    crate->Translate(glm::vec3(-3,1,0));

    mEntities.push_back(shared_ptr<Entity>(crate));

    Entity* house = new Entity();
    if (!house->AddMesh("resources/CasaSimples.obj", true)) {
        cerr << "Error loading: CasaSimples.obj" << endl;
    }

    house->Translate(glm::vec3(-10,2,0));

    mEntities.push_back(shared_ptr<Entity>(house));

    Entity* tree = new Entity();
    if (!tree->AddMesh("resources/trees9.3ds", true)) {
        cerr << "Error loading: trees9.3ds" << endl;
    }

    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> tree_loc(10, 20);
    uniform_int_distribution<> tree_type(0,7);
    uniform_real_distribution<> tree_size(0.3, 1.0);

    vector<glm::mat4> world_matrices;
    vector<glm::mat4> rot_matrices;
    vector<unsigned int> tree_types;
    glm::vec3 curPos(-50.0, -50.0, 0.0);
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            glm::mat4 world_matrix;
            world_matrix = glm::scale(glm::mat4(), glm::vec3(tree_size(gen), tree_size(gen), tree_size(gen)));
            world_matrix = world_matrix * glm::rotate(glm::mat4(), -90.0f, glm::vec3(1,0,0));

            glm::mat4 rot_matrix = world_matrix;
            world_matrix *= glm::translate(glm::mat4(), curPos);

            curPos += glm::vec3(0.0f, tree_loc(gen), 0.0f);
            world_matrices.push_back(world_matrix);
            rot_matrices.push_back(rot_matrix);
            tree_types.push_back(tree_type(gen));
        }
        curPos = glm::vec3(-50.0 + tree_loc(gen), -50.0, 0.0f);        
    }

    mGraphics->AttachTreeMatrices(shared_ptr<Entity>(tree), world_matrices, rot_matrices, tree_types);
}