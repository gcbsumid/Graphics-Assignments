#include <stdexcept>
#include <iostream>
#include <string>
#include <cassert>

#include "GraphicsManager.hpp"

using namespace std;

GraphicsManager::GraphicsManager(SDL_Window* window) 
    : mWindow(window)
{

}

GraphicsManager::~GraphicsManager() {
    // for (auto& shader : mShaders) {
    //     delete shader;
    // }
}

// int GraphicsManager::LoadShaders(string vert_shader, string frag_shader) {
//     vector<Shader> shaders;

//     shaders.push_back(Shader::ShaderFromFile(vert_shader, GL_VERTEX_SHADER));
//     shaders.push_back(Shader::ShaderFromFile(frag_shader, GL_FRAG_SHADER));

//     mShaders.push_back(new Program(shaders));
//     return (mShaders.size() - 1);
// }

// void GraphicsManager::SetActiveShader(int id) {
//     assert(mShaders.size() > id && id >= 0);

//     if (mActiveShader != NULL) {
//         mActiveShader->Stop();
//     }
//     mActiveShader = mShaders.at(id);
//     mActiveShader->Use();
// }

bool GraphicsManager::Render() {
    // assert(mActiveShader != NULL);

    glClearColor(1,1,1,1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // TODO: Redo a lot of this

    // for (auto& drawCompPtr : mDrawComponents) {

    //     auto drawComp = drawCompPtr.lock();
    //     if (!drawComp)
    //         throw std::runtime_error("Draw Component couldn't be locked when trying to render it.");

    //     // If the shader for the object isn't active
    //     if (!mActiveShader->IsInUse() || drawComp->GetShader() != mActiveShader) {
    //         mActiveShader->Stop();
    //         mActiveShader = drawComp->GetShader();
    //         mActiveShader->Use(); 

    //         // Set the Camera Variables for the Shader
    //         auto camera = mCamera.lock();
    //         if (!camera)
    //             throw std::runtime_error("Camera Component couldn't be locked when trying to render it.");
            
    //         if (!camera->Render(mActiveShader))
    //             throw std::runtime_error("The Camera Component wasn't Rendered properly.");

    //         // Render Light Components
    //         if (mInput->GetLightStatus()) {
    //             for (auto it : mLightComponents) {
    //                 if (auto lightComp = (it).lock()) {
    //                     if (!lightComp->Render(mActiveShader))
    //                         throw std::runtime_error("The Light Component wasn't Rendered properly.");
    //                 } else {
    //                     throw std::runtime_error("Light Component couldn't be locked when trying to render it.");
    //                 }
    //             }
    //         } else {
    //             // TODO: flashlight light
    //         }
    //     }

    //     std::string materialName = drawComp->GetMaterialName();
    //     mTextures->at(materialName)->Bind(mActiveShader);

    //     if (!drawComp->Render(mActiveShader)) {
    //         throw std::runtime_error("The Draw Component wasn't drawn properly.");
    //     }

    //     mTextures->at(materialName)->Unbind();
    // }

    auto skybox = mSkybox.lock();
    skybox->Render();

    SDL_GL_SwapWindow(mWindow);
    return true;
}

void GraphicsManager::AttachCamera(shared_ptr<Camera> camera) {
    mCamera = camera;
}

// TODO: render Skybox
void GraphicsManager::AttachSkybox(shared_ptr<Skybox> skybox) {
    mSkybox = skybox;
}

// TODO: remove Generate Light Component
// void GraphicsManager::GenerateLightComp(shared_ptr<Entity> ent, 
//     glm::vec3 color,
//     glm::vec3 intensities,
//     float attenuation,
//     float ambientCoef)
// {
//     ent->AddComponent(new LightComp(ent, color, intensities, attenuation, ambientCoef));
//     weak_ptr<LightComp> lightcomp = static_pointer_cast<LightComp>(ent->GetComponent(Component::COMPTYPE_LIGHT));
//     mLightComponents.push_back(lightcomp);
// }

void GraphicsManager::LinkInputManager(shared_ptr<InputManager> input) {
    assert(input.use_count());
    mInput = input;
}

// void GraphicsManager::LinkResourceManager(shared_ptr<ResourceManager> resource) {
//     assert(resource.use_count());
//     mResource = resource;
//     resource->SetTextureSharedPtr(mTextures);
//     resource->SetMeshSharedPtr(mMeshes);
// }

// void GraphicsManager::AttachShaderToEntity(shared_ptr<Entity> ent, int id) {
//     assert(mShaders.size() > id && id >= 0);
//     assert(ent->GetComponent(Component::COMPTYPE_DRAW) != nullptr);
    
//     shared_ptr<DrawComp> comp = static_pointer_cast<DrawComp>(ent->GetComponent(Component::COMPTYPE_DRAW));
//     comp->SetShader(mShaders.at(id));
// }

// void GraphicsManager::AttachMeshToEntity(shared_ptr<Entity> ent, shared_ptr<Mesh> mesh) {
//     // assert(mMeshes->size() > id && id >= 0);
//     assert(ent.count() && mesh.count());
//     assert(ent->GetComponent(Component::COMPTYPE_DRAW) != nullptr);
    
//     shared_ptr<DrawComp> comp = static_pointer_cast<DrawComp>(ent->GetComponent(Component::COMPTYPE_DRAW));
//     comp->SetMesh(mesh);
// }