#include <iostream>
#include "Skybox.hpp"
#include "Pipeline.hpp"

using namespace std;

Skybox::Skybox(const std::shared_ptr<Camera> camera) 
    : mCamera(camera) {
    // , mDrawType(GL_TRIANGLES)
    // , mDrawStart(0)
    // , mDrawCount(6*2*3) {
    // mCamera = camera;
}

Skybox::~Skybox() {
} 
 
bool Skybox::Init(const string& PosXFilename,
          const string& NegXFilename,
          const string& PosYFilename,
          const string& NegYFilename,
          const string& PosZFilename,
          const string& NegZFilename) {



    // Create the Skybox Technique
    mSkyboxTech = std::shared_ptr<SkyboxTechnique>(new SkyboxTechnique());
    if (!mSkyboxTech->Init()) {
        cerr << "Error at Skybox Technique initialization." << endl;
    }
    mSkyboxTech->Enable();
    mSkyboxTech->SetTextureUnit(0);


    mCubemapTexture = std::shared_ptr<CubemapTexture> (
        new CubemapTexture(PosXFilename,
           NegXFilename,
           PosYFilename,
           NegYFilename,
           PosZFilename,
           NegZFilename));

    if (!mCubemapTexture->Load()) {
        cerr << "Error: Unable to load texture." << endl;
        return false;
    }

    mSkyboxMesh = std::shared_ptr<Mesh>(new Mesh(false));
    return mSkyboxMesh->LoadMesh("resources/sphere.obj"); 
}

bool Skybox::Render() {
    mSkyboxTech->Enable();

    GLint OldCullFaceMode;
    glGetIntegerv(GL_CULL_FACE_MODE, &OldCullFaceMode);
    GLint OldDepthFuncMode;
    glGetIntegerv(GL_DEPTH_FUNC, &OldDepthFuncMode);    

    glCullFace(GL_FRONT);
    glDepthFunc(GL_LEQUAL);

    auto camera = mCamera.lock();

    Pipeline pipeline;
    // pipeline.SetPerspective(glm::transpose(camera->GetGLMPerspMatrix()));
    pipeline.SetPerspective(camera->GetPerspMatrix());
    pipeline.SetCamera(camera->GetPos(), camera->Forward(), camera->Up());

    const glm::mat4 transform = glm::transpose(GetTransform());
    
    mSkyboxTech->SetTextureUnit(0);
    mSkyboxTech->SetWVP(pipeline.GetWVPTrans(transform));

    mCubemapTexture->Bind(mSkyboxTech->GetProgram());
    mSkyboxMesh->Render(mSkyboxTech->GetProgram());
    mCubemapTexture->Unbind();

    glCullFace(OldCullFaceMode);
    glDepthFunc(OldDepthFuncMode);
    return true;
}