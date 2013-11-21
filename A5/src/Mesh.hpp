#ifndef MESH_H
#define MESH_H

// GLM Library
#include <glm/glm.hpp>

// Standard Library
#include <string>
#include <vector>

#include "Texture.hpp"
#include <assimp/scene.h>
// #include "AABB.h"

struct Vertex {
    // float x, y, z;    // (x,y,z)
    glm::vec3 mPosition;
    glm::vec2 mTexCoord;    // (u,v)
    glm::vec3 mNormal;      // (x,y,z)

    Vertex() {};
    Vertex(const glm::vec3& pos, const glm::vec2& tex, const glm::vec3 normal) {
        // x = pos.x;
        // y = pos.y;
        // z = pos.z;
        mPosition = pos;
        mTexCoord = tex;
        mNormal = normal;
    }
};

class Mesh
{
public:
    Mesh();

    ~Mesh();

    bool LoadMesh(const std::string& Filename);

    void Render(std::shared_ptr<Program> shader);

private:
    bool InitFromScene(const aiScene* pScene, const std::string& Filename);
    void InitMesh(unsigned int Index, const aiMesh* paiMesh);
    bool InitMaterials(const aiScene* pScene, const std::string& Filename);
    void SetMaterialData(const aiMaterial* material, Texture* texture);
    void Clear();

#define INVALID_MATERIAL 0xFFFFFFFF

    struct MeshEntry {
        MeshEntry();

        ~MeshEntry();

        bool Init(const std::vector<Vertex>& Vertices,
                  const std::vector<unsigned int>& Indices);

        GLuint mVertexBuffer;
        GLuint mIndexBuffer;
        unsigned int mNumIndices;
        unsigned int mMaterialIndex;
    };

    std::vector<MeshEntry> mEntries;
    std::vector<Texture*> mTextures;
};


// struct Mesh {
//     Mesh();
//     ~Mesh();

//     bool LoadMesh(const std::string& Filename);

//     void Render();

//     void Init(const std::vector<Vertex>& vertices, 
//               const std::vector<unsigned int>& indices);
//               // const AABB& box);
    
//     int mID;
//     // std::string mName;
//     GLuint mVertexBuffer;
//     GLuint mVertexArray;
//     GLuint mIndexBuffer;

//     // AABB mBoundingBox;

//     unsigned int mNumIndices; 
//     std::string mMaterialName;        
// };

#endif