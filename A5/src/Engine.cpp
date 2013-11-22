#include <chrono>
#include <string>
#include <stdexcept>
#include <iostream>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <GL/glfw.h>

#include "Engine.hpp"

using namespace std;

// TODO: Implement object creations
const glm::vec2 SCREEN_SIZE(800, 600);

Engine::Engine() {

    if (!glfwInit()) 
        throw std::runtime_error("glfwInit failed.");


    // open a window in GLFW
    glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 3);
    glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 3);
    glfwOpenWindowHint(GLFW_WINDOW_NO_RESIZE, GL_TRUE);
    if (!glfwOpenWindow(512, 512, 8, 8, 8, 8, 16, 0, GLFW_WINDOW))
        throw std::runtime_error("glfwOpenWindow failed. Hardware can't handle OpenGL 3.3");

    // initialise GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
        throw std::runtime_error("glewInit failed");

    // initialize GLEW
    if (!GLEW_VERSION_3_3) 
        throw std::runtime_error("OpenGL 3.3 Api is not available.");

    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;

    glfwDisable(GLFW_MOUSE_CURSOR);
    glfwSetMousePos(0,0);

    // if ( SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO ) < 0 ) {
    //     SDL_Quit();
    //     throw runtime_error("Could not initialize SDL\n\n");
    // }


    // SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    // SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

    // // Turn on double buffering with 24 bit Z buffer
    // SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    // SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    // mMainWindow = SDL_CreateWindow("Slender Boy", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
    //     512, 512, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    // if (!mMainWindow) {
    //     SDL_Quit();
    //     throw runtime_error("Unable to Create window.");
    // } 
 
    // /* Create our opengl context and attach it to our window */
    // mMainContext = SDL_GL_CreateContext(mMainWindow);

    // /* This makes our buffer swap syncronized with the monitor's vertical refresh */
    // SDL_GL_SetSwapInterval(1);

    // GLenum err = glewInit();
    // if (err != GLEW_OK) {
    //     cerr << "Init glew error." << endl;        
    //     SDL_Quit();
    // }

    // if (glewIsSupported("GL_VERSION_3_2"))
    //     printf("Ready for OpenGL 3.3\n");
    // else {
    //     printf("OpenGL 3.2 not supported\n");
    //     SDL_Quit();
    // }

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
        // SDL_Event event;
        // if (SDL_PollEvent(&event)) {
        //     switch (event.type) {
        //         case  SDL_KEYDOWN:
        //             mInput->HandleKeyPress(event.key);
        //             break;
        //         case SDL_MOUSEMOTION:
        //             SDL_PeepEvents(&event, 9, SDL_GETEVENT, SDL_MOUSEMOTION, SDL_MOUSEMOTION);
        //             mInput->HandleMouseMotion(event.motion);
        //             break;
        //         case SDL_MOUSEBUTTONDOWN:
        //         case SDL_MOUSEBUTTONUP:
        //             mInput->HandleMouseButton(event.button);
        //             break;
        //         case SDL_QUIT:
        //             done = true;
        //             break;
        //         default:
        //             break;
        //     }

            
        //     const Uint8* keys = SDL_GetKeyboardState(NULL);
        //     if (keys[SDL_SCANCODE_ESCAPE]) {
        //         break;
        //     }
        // }
        // lastFrame = currentFrame;

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

    mCamera->Translate(glm::vec3(0.0, 1.0, -20.0));

    CreateSkybox();
}

void Engine::CreateSkybox() {
    // Create skybox and attach to the Graphics manager
    mSkybox = shared_ptr<Skybox>(new Skybox(mCamera));
    mGraphics->AttachSkybox(mSkybox);

    mSkybox->Init(
        // "resources/nightsky_right.jpg",
        // "resources/nightsky_left.jpg",
        // "resources/nightsky_top.jpg",
        // "resources/nightsky_top.jpg",
        // "resources/nightsky_front.jpg",
        // "resources/nightsky_back.jpg");
        "resources/sp3right.jpg",
        "resources/sp3left.jpg",
        "resources/sp3top.jpg",
        "resources/sp3bot.jpg",
        "resources/sp3front.jpg",
        "resources/sp3back.jpg");

    mSkybox->Scale(glm::vec3(20.0f, 20.0f, 20.0f));
    mSkybox->Translate(mCamera->GetPos());

    // TODO: SKYBOX
}