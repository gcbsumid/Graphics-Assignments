#include <stdexcept>
#include <iostream>
#include <string>
#include <cassert>
#include <GL/glew.h>
#include <GL/glfw.h>

#include "GraphicsManager.hpp"
#include "Pipeline.hpp"

using namespace std;

GraphicsManager::GraphicsManager() 
    : mLightingTechnique(new LightingTechnique("./lighting.vert", "./lighting.frag"))
    , mNullTech(new NullTechnique())
    , mShadowVolTech(new ShadowVolumeTechnique())
    , mStaticTech(new StaticTechnique())
{
}

GraphicsManager::~GraphicsManager() {
}

bool GraphicsManager::DisplayStatic() {
    glClearColor(0,0,0,1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    mStaticTech->Render();

    glfwSwapBuffers();

    return true;
}

bool GraphicsManager::Render() {
    // assert(mActiveShader != NULL);

    glClearColor(0,0,0,1.0);

    auto input = mInput.lock();

    if (input->GetShadowStatus()) {

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        glDepthMask(GL_TRUE);
        // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        RenderSceneIntoDepth();
        
        glEnable(GL_STENCIL_TEST);

        // Note: currently, they only contain one each
        for (auto& point_light : mPointLights) {
            if (input->GetLightStatus()) {
                point_light->mAmbientIntensity = 0.0f;
                point_light->mDiffuseIntensity = 0.8f;
            } else {
                point_light->mAmbientIntensity = 0.0f;
                point_light->mDiffuseIntensity = 0.0f;
            }

            RenderShadowVolumeIntoStencil(point_light->mPosition);
            RenderShadowedScene(true);
        }

        for (auto& spot_light : mSpotLights) {
            spot_light->mAmbientIntensity = 0.0f;
            spot_light->mDiffuseIntensity = 0.8f;
            // TODO: probably do this in the input
            auto camera = mCamera.lock();
            if (input->GetFlashlight()) {
                spot_light->mPosition = camera->GetPos();
                spot_light->mDirection = camera->Forward();
            } 

            RenderShadowVolumeIntoStencil(spot_light->mPosition);
            RenderShadowedScene(false);
        }
                
        glDisable(GL_STENCIL_TEST);
        
        RenderAmbientLight();
    } else {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        
        RenderNormalScene();
    }

    if (mSkybox.use_count()) {
        auto skybox = mSkybox.lock();
        skybox->Render();
    }

    glfwSwapBuffers();

    return true;
}

void GraphicsManager::RenderSceneIntoDepth() {
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    // glDrawBuffer(GL_NONE);
    glDepthMask(GL_TRUE);
    glEnable(GL_CULL_FACE);
    glDepthRange(0.1, 1.0);
    // glCullFace(GL_BACK);
          
    mNullTech->Enable();

    Pipeline p;
    auto camera = mCamera.lock();
    // p.SetPerspective(glm::transpose(camera->GetGLMPerspMatrix()));
    p.SetPerspective(camera->GetPerspMatrix());
    p.SetCamera(camera->GetPos(), camera->Forward(), camera->Up());

    for (unsigned int i = 0; i < mTreesWorldMatrices.size(); ++i) {
        mNullTech->SetWVP(p.GetWVPTrans(glm::transpose(mTreesWorldMatrices.at(i))));

        if (mTreeTypes.at(i) == 2) {
            mTrees->RenderEntry(mNullTech->GetProgram(), 2);
            mTrees->RenderEntry(mNullTech->GetProgram(), 8);
        } else {
            mTrees->RenderEntry(mNullTech->GetProgram(), mTreeTypes.at(i));
        }
    }

    for (auto& ent : mGameObjects) {
        mNullTech->SetWVP(p.GetWVPTrans(glm::transpose(ent->GetTransform())));
        ent->Render(mNullTech->GetProgram());
    }

    if (mGround.use_count()) {
        auto ground = mGround.lock();
        mNullTech->SetWVP(p.GetWVPTrans(glm::transpose(ground->GetTransform())));
        ground->RenderMesh(mNullTech->GetProgram());
    }
    glDepthMask(GL_FALSE);
}

void GraphicsManager::RenderShadowVolumeIntoStencil(glm::vec3& light_pos) {
    glClear(GL_STENCIL_BUFFER_BIT);      
    glDisable(GL_BLEND);

    // glDrawBuffer(GL_NONE);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    glDepthFunc(GL_LESS);
    // glDepthFunc(GL_LEQUAL);
    
    glDisable(GL_CULL_FACE);
                
    // We need the stencil test to be enabled but we want it
    // to succeed always. Only the depth test matters.
    glStencilFunc(GL_ALWAYS, 0, 0xFF);
    // glStencilFunc(GL_ALWAYS, 0, 0);

    glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
    glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP); 
            
    mShadowVolTech->Enable();

    mShadowVolTech->SetLightPos(light_pos);

    Pipeline p;
    auto camera = mCamera.lock();
    // p.SetPerspective(glm::transpose(camera->GetGLMPerspMatrix()));
    p.SetPerspective(camera->GetPerspMatrix());
    p.SetCamera(camera->GetPos(), camera->Forward(), camera->Up());
    mShadowVolTech->SetPerspectiveMatrix(p.GetPerspectiveCameraTrans());

    for (unsigned int i = 0; i < mTreesWorldMatrices.size(); ++i) {
        mShadowVolTech->SetModelMatrix(glm::transpose(mTreesWorldMatrices.at(i)));

        if (mTreeTypes.at(i) == 2) {
            mTrees->RenderEntry(mShadowVolTech->GetProgram(), 2);
            mTrees->RenderEntry(mShadowVolTech->GetProgram(), 8);
        } else {
            mTrees->RenderEntry(mShadowVolTech->GetProgram(), mTreeTypes.at(i));
        }
    }

    for (auto& ent : mGameObjects) {
        if (!ent->HasAdjacency()){
            continue;
        }
        mShadowVolTech->SetModelMatrix(glm::transpose(ent->GetTransform()));
        ent->Render(mShadowVolTech->GetProgram());
    }

    if (mGround.use_count()) {
        auto ground = mGround.lock();
        mShadowVolTech->SetModelMatrix(glm::transpose(ground->GetTransform()));
        // mShadowVolTech->SetWVP(p.GetWVPTrans(glm::transpose(ground->GetTransform())));
        ground->RenderMesh(mShadowVolTech->GetProgram());
    }
    
    glEnable(GL_CULL_FACE);
}

void GraphicsManager::RenderShadowedScene(bool IsPoint) {
    // glDrawBuffer(GL_BACK);
    glDepthFunc(GL_LEQUAL);
    glCullFace(GL_BACK);
    glEnable(GL_BLEND);  

    // glBlendEquation(GL_FUNC_ADD);
    // glBlendFunc(GL_ONE, GL_ONE);
                                
    // prevent update to the stencil buffer
    // glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glStencilOpSeparate(GL_BACK, GL_KEEP, GL_KEEP, GL_KEEP);
    glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_KEEP, GL_KEEP);
    glStencilFunc(GL_EQUAL, 0x0, 0xFF);

    Pipeline p;
    auto camera = mCamera.lock();
    // p.SetPerspective(glm::transpose(camera->GetGLMPerspMatrix()));
    p.SetPerspective(camera->GetPerspMatrix());
    p.SetCamera(camera->GetPos(), camera->Forward(), camera->Up());

    mLightingTechnique->Enable();
    mLightingTechnique->SetCameraPosition(camera->GetPos() - (-2.0f * camera->Forward()));
    mLightingTechnique->SetColorTextureUnit(0);

    if (IsPoint) {
        mLightingTechnique->SetPointLights(1, mPointLights);
    } else {
        mLightingTechnique->SetSpotLights(1, mSpotLights);
    }

    // Draw the trees
    for (unsigned int i = 0; i < mTreesWorldMatrices.size(); ++i) {
        mLightingTechnique->SetModelMatrix(glm::transpose(mTreesWorldMatrices.at(i)));
        mLightingTechnique->SetRotMatrix(glm::transpose(mTreesRotMatrices.at(i)));

        if (mTreeTypes.at(i) == 2) {
            mTrees->RenderEntry(mLightingTechnique->GetProgram(), 2);
            mTrees->RenderEntry(mLightingTechnique->GetProgram(), 8);
        } else {
            mTrees->RenderEntry(mLightingTechnique->GetProgram(), mTreeTypes.at(i));
        }
    }

    // mLightingTechnique->SetMatSpecularIntensity(0.5);
    // mLightingTechnique->SetShininess(0.5);
    for (auto& ent : mGameObjects) {
        mLightingTechnique->SetModelMatrix(glm::transpose(ent->GetTransform()));
        glm::mat4 rotate = ent->GetScale() * ent->GetRotate();
        mLightingTechnique->SetRotMatrix(glm::transpose(rotate));

        ent->Render(mLightingTechnique->GetProgram());
    }

    if (mGround.use_count()) {
        auto ground = mGround.lock();
        ground->Render();
    }

    // glDisable(GL_BLEND);
}

void GraphicsManager::RenderAmbientLight() {
    // glDrawBuffer(GL_BACK);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);
    
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_ONE, GL_ONE);

    auto input = mInput.lock();

    for (unsigned int i = 0; i < mPointLights.size(); ++i) {
        if (input->GetLightStatus()) {
            mPointLights.at(i)->mAmbientIntensity = 0.1f;
            mPointLights.at(i)->mDiffuseIntensity = 0.0f;
        } else {
            mPointLights.at(i)->mAmbientIntensity = 0.0f;
            mPointLights.at(i)->mDiffuseIntensity = 0.0f;            
        }
    }
    for (unsigned int i = 0; i < mSpotLights.size(); ++i) {
        mSpotLights.at(i)->mAmbientIntensity = 0.1f;
        mSpotLights.at(i)->mDiffuseIntensity = 0.0f;
    }

    Pipeline p;
    auto camera = mCamera.lock();
    // p.SetPerspective(glm::transpose(camera->GetGLMPerspMatrix()));
    p.SetPerspective(camera->GetPerspMatrix());
    p.SetCamera(camera->GetPos(), camera->Forward(), camera->Up());

    mLightingTechnique->Enable();
    mLightingTechnique->SetCameraPosition(camera->GetPos() - (-2.0f * camera->Forward()));
    mLightingTechnique->SetColorTextureUnit(0);
    mLightingTechnique->SetPointLights(mPointLights.size(), mPointLights);
    mLightingTechnique->SetSpotLights(mSpotLights.size(), mSpotLights);
    mLightingTechnique->SetPerspectiveMatrix(p.GetPerspectiveCameraTrans());

    for (unsigned int i = 0; i < mTreesWorldMatrices.size(); ++i) {
        mLightingTechnique->SetModelMatrix(glm::transpose(mTreesWorldMatrices.at(i)));
        mLightingTechnique->SetRotMatrix(glm::transpose(mTreesRotMatrices.at(i)));

        if (mTreeTypes.at(i) == 2) {
            mTrees->RenderEntry(mLightingTechnique->GetProgram(), 2);
            mTrees->RenderEntry(mLightingTechnique->GetProgram(), 8);
        } else {
            mTrees->RenderEntry(mLightingTechnique->GetProgram(), mTreeTypes.at(i));
        }
    }

    for (auto& ent : mGameObjects) {
        mLightingTechnique->SetModelMatrix(glm::transpose(ent->GetTransform()));
        glm::mat4 rotate = ent->GetScale() * ent->GetRotate();
        mLightingTechnique->SetRotMatrix(glm::transpose(rotate));

        ent->Render(mLightingTechnique->GetProgram());
    }

    if (mGround.use_count()) {
        auto ground = mGround.lock();
        ground->Render();
    }
        
    glDisable(GL_BLEND);
} 

void GraphicsManager::RenderNormalScene() {
    auto camera = mCamera.lock();
    auto input = mInput.lock();
    for (unsigned int i = 0; i < mPointLights.size(); ++i) {
        if (input->GetLightStatus()) {
            mPointLights.at(i)->mAmbientIntensity = 0.0f;
            mPointLights.at(i)->mDiffuseIntensity = 0.8f;
        } else {
            mPointLights.at(i)->mAmbientIntensity = 0.0f;
            mPointLights.at(i)->mDiffuseIntensity = 0.0f;

        }
    }
    for (unsigned int i = 0; i < mSpotLights.size(); ++i) {
        mSpotLights.at(i)->mAmbientIntensity = 0.0f;
        mSpotLights.at(i)->mDiffuseIntensity = 0.8f;


        if (input->GetFlashlight()) {
            mSpotLights.at(i)->mPosition = camera->GetPos();
            mSpotLights.at(i)->mDirection = camera->Forward();
        } 
    }

    Pipeline p;
    p.SetPerspective(camera->GetPerspMatrix());
    p.SetCamera(camera->GetPos(), camera->Forward(), camera->Up());

    mLightingTechnique->Enable();
    mLightingTechnique->SetCameraPosition(camera->GetPos() - (-2.0f * camera->Forward()));
    mLightingTechnique->SetColorTextureUnit(0);
    mLightingTechnique->SetPointLights(mPointLights.size(), mPointLights);
    mLightingTechnique->SetSpotLights(mSpotLights.size(), mSpotLights);
    mLightingTechnique->SetPerspectiveMatrix(p.GetPerspectiveCameraTrans());


    for (unsigned int i = 0; i < mTreesWorldMatrices.size(); ++i) {
        mLightingTechnique->SetModelMatrix(glm::transpose(mTreesWorldMatrices.at(i)));
        mLightingTechnique->SetRotMatrix(glm::transpose(mTreesRotMatrices.at(i)));

        if (mTreeTypes.at(i) == 2) {
            mTrees->RenderEntry(mLightingTechnique->GetProgram(), 2);
            mLightingTechnique->SetModelMatrix(glm::transpose(mTreesWorldMatrices.at(i)));
            mLightingTechnique->SetRotMatrix(glm::transpose(mTreesRotMatrices.at(i)));
            mTrees->RenderEntry(mLightingTechnique->GetProgram(), 8);
        } else {
            mTrees->RenderEntry(mLightingTechnique->GetProgram(), mTreeTypes.at(i));
        }
    }

    // mLightingTechnique->SetMatSpecularIntensity(0.5);
    // mLightingTechnique->SetShininess(0.5);
    for (auto& ent : mGameObjects) {
        mLightingTechnique->SetModelMatrix(glm::transpose(ent->GetTransform()));
        glm::mat4 rotate = ent->GetScale() * ent->GetRotate();
        mLightingTechnique->SetRotMatrix(glm::transpose(rotate));

        ent->Render(mLightingTechnique->GetProgram());
    }

    if (mGround.use_count()) {
        auto ground = mGround.lock();
        ground->Render();
    }
}

void GraphicsManager::AttachGameObject(std::shared_ptr<Entity> ent) {
    mGameObjects.push_back(ent);
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

void GraphicsManager::AttachTreeMatrices(std::shared_ptr<Entity> tree,
                                         std::vector<glm::mat4>& world_matrices,
                                         std::vector<glm::mat4>& rot_matrices,  
                                         std::vector<unsigned int>& types) {
    mTrees = tree;
    mTreesWorldMatrices = world_matrices;
    mTreesRotMatrices = rot_matrices;
    mTreeTypes = types;
}
