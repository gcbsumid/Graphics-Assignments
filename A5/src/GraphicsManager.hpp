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
// #include "Program.hpp"
// #include "Texture.hpp"
// #include "Mesh.hpp"

class GraphicsManager {
public:
    GraphicsManager();
    virtual ~GraphicsManager();

    void LoadShaders();

    bool Render();

    // These functions generate the component add adds it to the entity passed to it
    void GenerateCameraComp(Entity*);
    void GenerateDrawComp(Entity*);
    void GenerateLightComp(Entity*);

    void LinkInputManager(InputManager*);
    void LinkResourceManager(ResourceManager*);

    // Generating these should be done in the constructor. Then link them to the resource manager
    // void SetTextureArray(std::shared_ptr<std::map<std::string, Texture*>>& textures);
    // void SetMeshArray(std::shared_ptr<std::map<std::string, Texture*>>& meshes);

    // void AttachShaderToDrawComp(DrawComp*, int);
    // void AttachMeshToDrawComp(DrawComp*, int);

private:
    // Pointers to other Managers;
    std::weak_ptr<InputManager> mInput;
    std::weak_ptr<ResourceManager> mResource;

    // List of Components 
    std::vector<std::weak_ptr<DrawComponent>> mDrawComponents;
    std::vector<std::weak_ptr<LightComponent>> mLightComponents;
    std::weak_ptr<CameraComponent> mCameraComponent;

    // Shared with the resource manager
    // std::shared_ptr<std::map<std::string, Texture*>> mTextures;
    // std::shared_ptr<std::map<int, std::shared_ptr<Mesh>>> mMeshes;
    
    // Program* mActiveShader;
    // std::vector<Program*> mShaders;


};

#endif