#ifndef MESH_HPP
#define MESH_HPP

// GLM Library
#include <glm/glm.hpp>

// Standard Library
#include <string>
#include <vector>
// #include <list>

#include "Texture.hpp"
#include <assimp/scene.h>
// #include "AABB.h"

struct WE_Vertex;
struct WE_Face;
struct WE_Edge;

struct WE_Vertex {
    std::vector<WE_Edge*> mEdges;

    void GetFaces(std::vector<WE_Face*>& faces);

    // Object Data
    glm::vec3 mPosition;
    glm::vec3 mAvgNormal;
    glm::vec2 mTexCoord;    // (u,v)

    unsigned int mIndex;

    bool IsOnBorder();

    WE_Vertex() : mPosition(glm::vec3(0,0,0))
             , mAvgNormal(glm::vec3(0,0,1))
             , mTexCoord(glm::vec2(0,0)) {}
};

struct WE_Face {
    std::vector<WE_Edge*> mEdges; 

    void GetVertices(std::vector<WE_Vertex*>& vertices);

    // Object Data
    glm::vec3 mFaceNormal;

    // pointers for subdivision 
    WE_Vertex* mFaceVertex;
    
    WE_Face() : mFaceNormal(glm::vec3(0,0,1)) 
              , mFaceVertex(nullptr) {}
};

struct WE_Edge {
    // Object Data
    WE_Vertex *mVert1, *mVert2;
    WE_Face *mFaceA, *mFaceB;
    WE_Edge *mPrevA, *mNextA, *mPrevB, *mNextB;

    // Subdivision data
    WE_Vertex* mMidPointVertex;

    bool IsOnBorder();

    WE_Edge() : mVert1(nullptr)
              , mVert2(nullptr)
              , mFaceA(nullptr)
              , mFaceB(nullptr)
              , mPrevA(nullptr)
              , mNextA(nullptr)
              , mPrevB(nullptr)
              , mNextB(nullptr)
              , mMidPointVertex(nullptr) {}
};

// struct Vertex {
//     // float x, y, z;    // (x,y,z)
//     glm::vec3 mPosition;
//     glm::vec2 mTexCoord;    // (u,v)
//     glm::vec3 mNormal;      // (x,y,z)

//     Vertex() {};
//     Vertex(const glm::vec3& pos, const glm::vec2& tex, const glm::vec3 normal) {
//         // x = pos.x;
//         // y = pos.y;
//         // z = pos.z;
//         mPosition = pos;
//         mTexCoord = tex;
//         mNormal = normal;
//     }
// };

class Mesh
{
public:
    Mesh();

    virtual ~Mesh();

    bool LoadMesh(const std::string& Filename);

    virtual void Render(std::shared_ptr<Program> shader);

    void Subdivide(int numOfIteration);

protected:

    void SubdivideMeshEntry(int numOfIteration, int index);

    void CreateFace(std::vector<WE_Edge*>& edges_list, 
                      std::vector<WE_Face*>& face_list, 
                      std::vector<WE_Vertex*>& face_vertices,
                      std::vector<unsigned int>& indices);

    void GetVertexNormal(WE_Vertex* vert, std::vector<glm::vec3>& normals);

    bool InitFromScene(const aiScene* pScene, const std::string& Filename);
    bool InitOpenGLData(std::vector<unsigned int>& indices, 
                        std::vector<glm::vec3>& positions,
                        std::vector<glm::vec3>& normals,
                        std::vector<glm::vec2>& texcoords);
    void InitMesh(unsigned int index, 
                  const aiMesh* mesh, 
                  std::vector<glm::vec3>& position,
                  std::vector<glm::vec3>& normal,
                  std::vector<glm::vec2>& texcoord,
                  std::vector<unsigned int>& indices);
    bool InitMaterials(const aiScene* pScene, const std::string& Filename);
    void SetMaterialData(const aiMaterial* material, Texture* texture);
    virtual void Clear();

#define INVALID_MATERIAL 0xFFFFFFFF
#define INVALID_OGL_VALUE 0xFFFFFFFF

// Buffer Data indices
#define INDEX_BUFFER    0
#define POSITION_VB     1
#define NORMAL_VB       2
#define TEXCOORD_VB     3
#define MAX_BUFFERS     4

// Vertex Attrib Positions
#define POSITION_LOCATION 0
#define NORMAL_LOCATION 1
#define TEXCOORD_LOCATION 2

    struct MeshEntry {
        MeshEntry();

        ~MeshEntry();

        std::vector<WE_Vertex*> mVertices;
        std::vector<WE_Face*> mFaces;
        std::vector<WE_Edge*> mEdges;
        // GLuint mVertexBuffer;
        // GLuint mIndexBuffer;
        unsigned int mNumIndices;
        unsigned int mMaterialIndex;
        unsigned int mBaseVertex;
        unsigned int mBaseIndex;
    };

    GLuint mVertexArray;
    GLuint mBuffers[MAX_BUFFERS];

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