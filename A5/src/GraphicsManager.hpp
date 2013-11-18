#ifndef GRAPHICSMANAGER_HPP
#define GRAPHICSMANAGER_HPP

#include <GL/glew.h>
#include <map>
#include <memory>
#include <vector>
#include <string>

#include "Entity.hpp"
#include "Component.hpp"
#include "InputManager.hpp"
#include "ResourceManager.hpp"
#include "Program.hpp"
#include "Texture.hpp"
#include "Mesh.hpp"

class GraphicsManager {
public:
    GraphicsManager();
    virtual ~GraphicsManager();

    int LoadShaders(std::string vert_shader, std::string frag_shader);
    void SetActiveShader(int id);

    bool Render();

    // These functions generate the component add adds it to the entity passed to it
    void GenerateCameraComp(std::shared_ptr<Entity>);
    void GenerateDrawComp(std::shared_ptr<Entity>);
    void GenerateLightComp(std::shared_ptr<Entity>, glm::vec3, glm::vec3, float, float);

    void LinkInputManager(std::shared_ptr<InputManager>);
    void LinkResourceManager(std::shared_ptr<ResourceManager>);

    void AttachShaderToEntity(std::shared_ptr<Entity> ent, int id);
    void AttachMeshToEntity(std::shared_ptr<Entity> ent, int id);

private:
    // Pointers to other Managers;
    std::weak_ptr<InputManager> mInput;
    std::weak_ptr<ResourceManager> mResource;

    // List of Components 
    std::vector<std::weak_ptr<DrawComp>> mDrawComponents;
    std::vector<std::weak_ptr<LightComp>> mLightComponents;
    std::weak_ptr<CameraComp> mCameraComponent;

    // Shared with the resource manager
    std::shared_ptr<std::map<std::string, Texture*>> mTextures;
    std::shared_ptr<std::map<int, std::shared_ptr<Mesh>>> mMeshes;
    
    Program* mActiveShader;
    std::vector<Program*> mShaders;


};

#endif