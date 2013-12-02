#ifndef GRAPHICSMANAGER_HPP
#define GRAPHICSMANAGER_HPP

#include <GL/glew.h>
#include <map>
#include <memory>
#include <vector>
#include <string>
// #include <SDL2/SDL.h>

#include "Entity.hpp"
#include "Component.hpp"
#include "Camera.hpp"
#include "InputManager.hpp"
// #include "ResourceManager.hpp"
#include "Program.hpp"
#include "Texture.hpp"
#include "Mesh.hpp"
#include "Skybox.hpp"
#include "Ground.hpp"

#include "LightingTechnique.hpp"
#include "NullTechnique.hpp"
#include "ShadowVolumeTechnique.hpp"
#include "StaticTechnique.hpp"

class GraphicsManager {
public:
    GraphicsManager();
    // GraphicsManager(SDL_Window* window);
    virtual ~GraphicsManager();

    // int LoadShaders(std::string vert_shader, std::string frag_shader);
    // void SetActiveShader(int id);

    bool Render();
    bool DisplayStatic();

    // These functions generate the component add adds it to the entity passed to it
    // void GenerateLightComp(std::shared_ptr<Entity>, glm::vec3, glm::vec3, float, float);

    void LinkInputManager(std::shared_ptr<InputManager>);
    // void LinkResourceManager(std::shared_ptr<ResourceManager>);

    // void AttachShaderToEntity(std::shared_ptr<Entity> ent, int id);
    // void AttachMeshToEntity(std::shared_ptr<Entity> ent, int id);

    void AttachCamera(std::shared_ptr<Camera>);
    void AttachSkybox(std::shared_ptr<Skybox>);
    void AttachGround(std::shared_ptr<Ground>);
    void AttachSpotLights(std::vector<std::shared_ptr<SpotLight>>& spotlight);
    void AttachPointLights(std::vector<std::shared_ptr<PointLight>>& pointLight);
    void AttachGameObject(std::shared_ptr<Entity>);
    void AttachTreeMatrices(std::shared_ptr<Entity> tree,
                            std::vector<glm::mat4>& world_matrices,
                            std::vector<glm::mat4>& rot_matrices, 
                            std::vector<unsigned int>& types);

private:
    void RenderSceneIntoDepth();
    void RenderShadowVolumeIntoStencil(glm::vec3& light_pos);
    void RenderShadowedScene(bool IsPoint);
    void RenderAmbientLight();
    void RenderNormalScene();

    // Pointers to other Managers;
    std::weak_ptr<InputManager> mInput;
    // std::weak_ptr<ResourceManager> mResource;

    // List of Components 
    std::vector<std::weak_ptr<DrawComp>> mDrawComponents;
    // std::vector<std::weak_ptr<LightComp>> mLightComponents;
    std::weak_ptr<Camera> mCamera;
    std::weak_ptr<Skybox> mSkybox;
    std::weak_ptr<Ground> mGround;
    std::vector<std::shared_ptr<Entity>> mGameObjects;

    std::vector<std::shared_ptr<SpotLight>> mSpotLights;
    std::vector<std::shared_ptr<PointLight>> mPointLights;

    std::shared_ptr<LightingTechnique> mLightingTechnique;
    std::shared_ptr<NullTechnique> mNullTech;
    std::shared_ptr<ShadowVolumeTechnique> mShadowVolTech;
    std::shared_ptr<StaticTechnique> mStaticTech;

    std::shared_ptr<Entity> mTrees;
    std::vector<glm::mat4> mTreesWorldMatrices;
    std::vector<glm::mat4> mTreesRotMatrices;
    std::vector<unsigned int> mTreeTypes;



    // Shared with the resource manager
    // std::shared_ptr<std::map<std::string, Texture*>> mTextures;
    // std::shared_ptr<std::map<int, std::shared_ptr<Mesh>>> mMeshes;
    
    // SDL_Window* mWindow;
    // std::vector<Program*> mShaders;


};

#endif