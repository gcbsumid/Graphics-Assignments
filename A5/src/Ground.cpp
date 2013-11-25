#include <iostream>
#include "Ground.hpp"
#include "Pipeline.hpp"

using namespace std;

Ground::Ground(const std::shared_ptr<Camera> camera,
       unsigned int width, 
       unsigned int length,
       double maxHeight,
       double minHeight) 
    : mCamera(camera)
    , mGroundTech(new GroundTechnique())
{
    if (!mGroundTech->Init()) {
        cerr << "Error at Ground Technique Initialization." << endl;
    }

    mGroundTech->Enable();
    // mGroundTech->SetTextureUnit(0); // I may not even need this.

    mGroundMesh = shared_ptr<GroundMesh>(new GroundMesh(width, length, maxHeight, minHeight));
    mGroundMesh->RunFractalAlgorithm(1);
}

Ground::~Ground() {

}



bool Ground::Render() {
    mGroundTech->Enable();
    auto camera = mCamera.lock();

    Pipeline pipeline;
    pipeline.SetPerspective(camera->GetPerspMatrix());
    pipeline.SetCamera(-camera->GetPos(), -camera->Forward(), camera->Up());

    const glm::mat4 transform = GetTransform();
    mGroundTech->SetWVP(pipeline.GetWVPTrans(transform));

    mGroundMesh->Render(mGroundTech->GetProgram());

    return true;
}