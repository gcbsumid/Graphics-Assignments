#ifndef RESOURCEMANAGER_HPP
#define RESOURCEMANAGER_HPP

#include <memory>
#include <map>
#include <vector>
#include <string>

#include "Texture.hpp"
#include "Mesh.hpp"

class ResourceManager {
public:
    ResourceManager();
    virtual ~ResourceManager();

    // void LoadAllFiles();

    void SetTextureSharedPtr(std::shared_ptr<std::map<std::string, Texture*>>& textures);
    void SetMeshSharedPtr(std::shared_ptr<std::map<int, std::shared_ptr<Mesh>>>& meshes);
    void LoadAssetFromFile(const std::string& file); 

private:
    void Clear();
    void InitMaterials(const aiScene*, const std::string&);
    void InitMesh(const aiMesh*);
    void SetMaterialData(const aiMaterial*, Texture*);
    void ProcessScene(const aiScene*);
    
    // This is shared with the Graphics manager
    std::shared_ptr<std::map<std::string, Texture*>> mTextures;
    std::shared_ptr<std::map<int, std::shared_ptr<Mesh>>> mMeshes;
    std::vector<std::string> mLocalTexture;
};

#endif