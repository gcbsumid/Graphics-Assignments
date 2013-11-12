#ifndef RESOURCEMANAGER_HPP
#define RESOURCEMANAGER_HPP

#include <assimp/scene.h>
#include <memory>
#include <map>
#include <vector>
#include <string>

// #include "../Renderer/Texture.h"
// #include "../Renderer/Mesh.h"

class ResourceManager {
public:
    ResourceManager();
    virtual ~ResourceManager();

    void LoadAllFiles();
private:
    void Clear();
    void InitMaterials(const aiScene*, const std::string&);
    void InitMesh(const aiMesh*);
    void SetMaterialData(const aiMaterial*, Texture*);
    void LoadAssetFromFile(const std::string& file); 
    void ProcessScene(const aiScene*);
    
    // This is shared with the Graphics manager
    std::shared_ptr<std::map<std::string, Texture*>> mTextures;
    std::shared_ptr<std::map<int, std::shared_ptr<Mesh>>> mMeshes;
    std::vector<std::string> mLocalTexture;
};

#endif