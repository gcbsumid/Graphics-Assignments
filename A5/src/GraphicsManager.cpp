#include <stdexcept>
#include <iostream>
#include <string>
#include <cassert>
#include <GL/glew.h>
#include <GL/glfw.h>

#include "GraphicsManager.hpp"

using namespace std;

GraphicsManager::GraphicsManager() 
    : mLightingTechnique(new LightingTechnique("./lighting.vert", "./lighting.frag"))
{
}

GraphicsManager::~GraphicsManager() {
}

bool GraphicsManager::Render() {
    // assert(mActiveShader != NULL);

    glClearColor(0,0,0,1.0);

    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 
    // RenderSceneIntoDepth();
    
    // glEnable(GL_STENCIL_TEST);
            
    // RenderShadowVolIntoStencil();
    
    RenderShadowedScene();
    
    // glDisable(GL_STENCIL_TEST);
    
    // RenderAmbientLight();
    
    glfwSwapBuffers();

    return true;
}

void GraphicsManager::RenderSceneIntoDepth() {
    // glDrawBuffer(GL_NONE);
    // glDepthMask(GL_TRUE);
          
    // mNullTech.Enable();

    // Pipeline p;
    // auto camera = mCamera.lock();
    // pipeline.SetPerspective(camera->GetPerspMatrix());
    // pipeline.SetCamera(camera->GetPos(), camera->Forward(), camera->Up());

    // auto ground = mGround.lock();
    // mNullTech->SetWVP(pipeline.GetWVPTrans(glm::transpose(ground->GetTransform())));
}

void GraphicsManager::RenderShadowVolumeIntoStencil() {
    // glDrawBuffer(GL_NONE);
    // glDepthMask(GL_FALSE);
    
    // glDisable(GL_CULL_FACE);
                
    // // We need the stencil test to be enabled but we want it
    // // to succeed always. Only the depth test matters.
    // glStencilFunc(GL_ALWAYS, 0, 0xff);

    // glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
    // glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);       
            
    // mShadowVolTech.Enable();

    // mShadowVolTech.SetLightPos(m_pointLight.Position);
           
    // Pipeline p;
    // p.SetCamera(m_pGameCamera->GetPos(), m_pGameCamera->GetTarget(), m_pGameCamera->GetUp());
    // p.SetPerspectiveProj(m_persProjInfo);                       
    // p.WorldPos(m_boxPos);        
    // p.Rotate(0, m_scale, 0);
    // m_ShadowVolTech.SetWorldMatrix(p.GetWorldTrans());
    // m_ShadowVolTech.SetVP(p.GetVPTrans());
    
    // m_box.Render();        
    
    // glEnable(GL_CULL_FACE);
}

void GraphicsManager::RenderShadowedScene() {

    for (unsigned int i = 0; i < mPointLights.size(); ++i) {
        mPointLights.at(i)->mAmbientIntensity = 0.2f;
        mPointLights.at(i)->mDiffuseIntensity = 0.8f;
    }

    mLightingTechnique->Enable();
    mLightingTechnique->SetColorTextureUnit(0);
    mLightingTechnique->SetPointLights(mPointLights.size(), mPointLights);
    mLightingTechnique->SetSpotLights(mSpotLights.size(), mSpotLights);

    if (mGround.use_count()) {
        auto ground = mGround.lock();
        ground->Render();
    }

    if (mSkybox.use_count()) {
        auto skybox = mSkybox.lock();
        skybox->Render();
    }
}

void GraphicsManager::RenderAmbientLight() {

}

void GraphicsManager::AttachCamera(shared_ptr<Camera> camera) {
    mCamera = camera;
}

void GraphicsManager::AttachSkybox(shared_ptr<Skybox> skybox) {
    mSkybox = skybox;
}

void GraphicsManager::AttachGround(shared_ptr<Ground> ground) {
    mGround = ground;
}

void GraphicsManager::AttachSpotLights(std::vector<std::shared_ptr<SpotLight>>& spotLights) {
    mSpotLights = spotLights;
}

void GraphicsManager::AttachPointLights(std::vector<std::shared_ptr<PointLight>>& pointLights) {
    mPointLights = pointLights;
}

void GraphicsManager::LinkInputManager(shared_ptr<InputManager> input) {
    assert(input.use_count());
    mInput = input;
}