#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <stdexcept>
#include "Ground.hpp"
#include "Pipeline.hpp"

using namespace std;

Ground::Ground(const std::shared_ptr<Camera> camera,
       unsigned int gridX, 
       unsigned int gridZ,
       double maxHeight,
       double minHeight) 
    : mCamera(camera)
    , mGroundTech(new LightingTechnique("./ground.vert", "./ground.frag"))
{
    // if (!mGroundTech->Init()) {
    //     cerr << "Error at Ground Technique Initialization." << endl;
    // }

    mGroundTech->Enable();
    // mGroundTech->SetTextureUnit(0); // I may not even need this.
    mGroundTech->SetMatSpecularIntensity(0.0f);
    mGroundTech->SetShininess(0.0f);      

    mGroundMesh = shared_ptr<GroundMesh>(new GroundMesh(gridX, gridZ, maxHeight, minHeight));

    mGroundMesh->Subdivide(1);

    // mGroundMesh->RunFractalAlgorithm(1);
}

Ground::~Ground() {
}

bool Ground::RenderMesh(std::shared_ptr<Program>& shader) {
    mGroundMesh->Render(shader);
    return true;
}

bool Ground::Render() {
    mGroundTech->Enable();

    glDisable(GL_CULL_FACE);

    auto camera = mCamera.lock();

    Pipeline pipeline;
    // pipeline.SetPerspective(glm::transpose(camera->GetGLMPerspMatrix()));
    pipeline.SetPerspective(camera->GetPerspMatrix());
    pipeline.SetCamera(camera->GetPos(), camera->Forward(), camera->Up());

    mGroundTech->SetColorTextureUnit(0);
    // for (int i = 0; i < mPointLights.size(); ++i) {
    //     cout << "Ambient: " << mPointLights.at(i)->mAmbientIntensity << endl;
    //     cout << "Diffuse: " << mPointLights.at(i)->mDiffuseIntensity << endl;
    // }

    // It is now done in the graphics manager
    // for (unsigned int i = 0; i < mSpotLights.size(); ++i) {
    //     mSpotLights.at(i)->mPosition = camera->GetPos();
    //     mSpotLights.at(i)->mDirection = camera->Forward();
    //     // cout << "Ambient: " << mPointLights.at(i)->mAmbientIntensity << endl;
    //     // cout << "Diffuse: " << mPointLights.at(i)->mDiffuseIntensity << endl;
    // }

    mGroundTech->SetCameraPosition(camera->GetPos());
    mGroundTech->SetPointLights(mPointLights.size(), mPointLights);
    mGroundTech->SetSpotLights(mSpotLights.size(), mSpotLights);
    mGroundTech->SetPerspectiveMatrix(pipeline.GetPerspectiveCameraTrans());
    mGroundTech->SetModelMatrix(glm::transpose(GetTransform()));
    glm::mat4 rotate = GetScale() * GetRotate();
    mGroundTech->SetRotMatrix(glm::transpose(rotate));

    // mGroundTech->SetRotMatrix(GetRotate());
    mGroundMesh->Render(mGroundTech->GetProgram());

    glEnable(GL_CULL_FACE);

    // cout << "Forward: " << camera->Forward().x << ", " << camera->Forward().y << ", " << camera->Forward().z << endl;

    return true;
}

void Ground::AttachSpotLights(std::vector<std::shared_ptr<SpotLight>>& spotLights) {
    mSpotLights = spotLights;
}

void Ground::AttachPointLights(std::vector<std::shared_ptr<PointLight>>& pointLights) {
    mPointLights = pointLights;
}

