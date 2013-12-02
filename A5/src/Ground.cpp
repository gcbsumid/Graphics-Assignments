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
    : mGridX(gridX)
    , mGridZ(gridZ)
    , mMaxHeight(maxHeight)
    , mMinHeight(minHeight) 
    , mHasSubdivided(false)
    , mCamera(camera)
    , mGroundTech(new LightingTechnique("./ground.vert", "./ground.frag"))
{
    // if (!mGroundTech->Init()) {
    //     cerr << "Error at Ground Technique Initialization." << endl;
    // }

    mGroundTech->Enable();
    mGroundTech->SetMatSpecularIntensity(0.0f);
    mGroundTech->SetShininess(0.0f);      

    mGroundMesh = shared_ptr<GroundMesh>(new GroundMesh(gridX, gridZ, maxHeight, minHeight, false));
}

Ground::~Ground() {
}

void Ground::Rebuild(bool withSubdivision, bool withFractal) {
    if (withSubdivision && !mHasSubdivided) {
        mHasSubdivided = true;
        mGroundMesh->Subdivide(2);
        return;
    } 

    mGroundMesh = shared_ptr<GroundMesh>(new GroundMesh(mGridX, mGridZ, mMaxHeight, mMinHeight, withFractal));
    mHasSubdivided = false;
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
    pipeline.SetPerspective(camera->GetPerspMatrix());
    pipeline.SetCamera(camera->GetPos(), camera->Forward(), camera->Up());

    mGroundTech->SetColorTextureUnit(0);

    mGroundTech->SetCameraPosition(camera->GetPos());
    mGroundTech->SetPointLights(mPointLights.size(), mPointLights);
    mGroundTech->SetSpotLights(mSpotLights.size(), mSpotLights);
    mGroundTech->SetPerspectiveMatrix(pipeline.GetPerspectiveCameraTrans());
    mGroundTech->SetModelMatrix(glm::transpose(GetTransform()));
    glm::mat4 rotate = GetScale() * GetRotate();
    mGroundTech->SetRotMatrix(glm::transpose(rotate));

    mGroundMesh->Render(mGroundTech->GetProgram());

    glEnable(GL_CULL_FACE);


    return true;
}

void Ground::AttachSpotLights(std::vector<std::shared_ptr<SpotLight>>& spotLights) {
    mSpotLights = spotLights;
}

void Ground::AttachPointLights(std::vector<std::shared_ptr<PointLight>>& pointLights) {
    mPointLights = pointLights;
}

