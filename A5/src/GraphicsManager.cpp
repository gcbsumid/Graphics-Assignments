#include <stdexcept>
#include <iostream>
#include <string>
#include <cassert>

#include "GraphicsManager.hpp"
#include "SDL_OGL.h" 

using namespace std;

GraphicsManager::GraphicsManager() 
    : mActiveShader(nullptr)
{

}

GraphicsManager::~GraphicsManager() {
    for (auto& shader : mShaders) {
        delete shader;
    }
}

int GraphicsManager::LoadShaders(string vert_shader, string frag_shader) {
    vector<Shader> shaders;

    shaders.push_back(Shader::ShaderFromFile(vert_shader, GL_VERTEX_SHADER));
    shaders.push_back(Shader::ShaderFromFile(frag_shader, GL_VERTEX_SHADER));

    mShaders.push_back(new Program(shaders));
    return (mShaders.size() - 1);
}

void GraphicsManager::SetActiveShader(int id) {
    assert(mShaders.size() > id && id >= 0);

    if (mActiveShader != NULL) {
        mActiveShader->Stop();
    }
    mActiveShader = mShaders.at(id);
    mActiveShader->Use();
}

bool GraphicsManager::Render() {
    assert(mActiveShader != NULL);

    glClearColor(1,1,1,1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (auto& drawCompPtr : mDrawComponents) {
        // if (drawCompPtr.expired()) {
        //     drawCompPtr = mDrawComponents.erase(drawCompPtr);
        //     continue;
        // }

        auto drawComp = drawCompPtr.lock();
        if (!drawComp)
            throw std::runtime_error("Draw Component couldn't be locked when trying to render it.");

        // If the shader for the object isn't active
        if (!mActiveShader->IsInUse() || drawComp->GetShader() != mActiveShader) {
            mActiveShader->Stop();
            mActiveShader = drawComp->GetShader();
            mActiveShader->Use(); 

            // Set the Camera Variables for the Shader
            auto cameraComp = mCameraComponent.lock();
            if (!cameraComp)
                throw std::runtime_error("Camera Component couldn't be locked when trying to render it.");
            
            if (!cameraComp->Render(mActiveShader))
                throw std::runtime_error("The Camera Component wasn't Rendered properly.");

            // Render Light Components
            for (auto it : mLightComponents) {
                if (auto lightComp = (it).lock()) {
                    if (!lightComp->Render(mActiveShader))
                        throw std::runtime_error("The Light Component wasn't Rendered properly.");
                } else {
                    throw std::runtime_error("Light Component couldn't be locked when trying to render it.");
                }
            }
            
        }

        // std::cout << "I am drawing component " << drawComp->GetID() << std::endl;

        std::string materialName = drawComp->GetMaterialName();
        mTextures->at(materialName)->BindTexture(mActiveShader);

        if (!drawComp->Render(mActiveShader)) {
            throw std::runtime_error("The Draw Component wasn't drawn properly.");
        }

        mTextures->at(materialName)->UnbindTexture();
    }

    SDL_GL_SwapBuffers();
}

void GraphicsManager::GenerateCameraComp(shared_ptr<Entity> ent) {
    ent->AddComponent(new CameraComp(ent));
    mCameraComponent = static_pointer_cast<CameraComp>(ent->GetComponent(Component::COMPTYPE_CAMERA));

    assert(mInput.use_count());
    auto input = mInput.lock();
    input->AttachCameraComponent(static_pointer_cast<CameraComp>(ent->GetComponent(Component::COMPTYPE_CAMERA)));
}

void GraphicsManager::GenerateDrawComp(shared_ptr<Entity> ent) {
    ent->AddComponent(new DrawComp(ent));
    weak_ptr<DrawComp> drawcomp = static_pointer_cast<DrawComp>(ent->GetComponent(Component::COMPTYPE_DRAW));
    mDrawComponents.push_back(drawcomp);
}

void GraphicsManager::GenerateLightComp(shared_ptr<Entity> ent, 
    glm::vec3 color,
    glm::vec3 intensities,
    float attenuation,
    float ambientCoef)
{
    ent->AddComponent(new LightComp(ent, color, intensities, attenuation, ambientCoef));
    weak_ptr<LightComp> lightcomp = static_pointer_cast<LightComp>(ent->GetComponent(Component::COMPTYPE_LIGHT));
    mLightComponents.push_back(lightcomp);
}

void GraphicsManager::LinkInputManager(shared_ptr<InputManager> input) {
    assert(input.use_count());
    mInput = input;
}

void GraphicsManager::LinkResourceManager(shared_ptr<ResourceManager> resource) {
    assert(resource.use_count());
    mResource = resource;
    resource->SetTextureSharedPtr(mTextures);
    resource->SetMeshSharedPtr(mMeshes);
}

void GraphicsManager::AttachShaderToEntity(shared_ptr<Entity> ent, int id) {
    assert(mShaders.size() > id && id >= 0);
    assert(ent->GetComponent(Component::COMPTYPE_DRAW) != nullptr);
    
    shared_ptr<DrawComp> comp = static_pointer_cast<DrawComp>(ent->GetComponent(Component::COMPTYPE_DRAW));
    comp->SetShader(mShaders.at(id));
}

void GraphicsManager::AttachMeshToEntity(shared_ptr<Entity> ent, int id) {
    assert(mMeshes->size() > id && id >= 0);
    assert(ent->GetComponent(Component::COMPTYPE_DRAW) != nullptr);
    
    shared_ptr<DrawComp> comp = static_pointer_cast<DrawComp>(ent->GetComponent(Component::COMPTYPE_DRAW));
    comp->SetMesh(mMeshes->at(id));
}