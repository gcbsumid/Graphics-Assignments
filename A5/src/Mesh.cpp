// GL Library
#include <GL/glew.h>

// Standard Library
#include <iostream>
#include <cassert>

// Backlash Library
#include "Mesh.hpp"
#include <assimp/scene.h>
#include <assimp/Importer.hpp> 
#include <assimp/postprocess.h>

using namespace std;

static const GLuint INVALID_OGL_VALUE = 0xFFFFFFFF;

static void color4_to_float4 (const aiColor4D *c, glm::vec4& f) {
    f = glm::vec4(c->r, c->g, c->b, c->a);
}

Mesh::MeshEntry::MeshEntry(){
    mVertexBuffer = INVALID_OGL_VALUE;
    mIndexBuffer = INVALID_OGL_VALUE;
    mNumIndices = 0;
    mMaterialIndex = INVALID_MATERIAL;
    // mMaterialName = "";
    // mName = "";
}

Mesh::MeshEntry::~MeshEntry() {
    if (mVertexBuffer != INVALID_OGL_VALUE) {
        glDeleteBuffers(1, &mVertexBuffer);
    }

    if (mIndexBuffer != INVALID_OGL_VALUE) {
        glDeleteBuffers(1, &mIndexBuffer);
    }
}

bool Mesh::MeshEntry::Init(const std::vector<Vertex>& vertices, 
                const std::vector<unsigned int>& indices) {
                // const AABB& box) {
    mNumIndices = indices.size();
    // mBoundingBox = box;

    glGenBuffers(1, &mIndexBuffer) ;
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * mNumIndices, &indices[0], GL_STATIC_DRAW);

    glGenBuffers(1, &mVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
    // glBindVertexArray(mVertexArray);
        
    glGenVertexArrays(1, &mVertexArray);
    glBindVertexArray(mVertexArray);
    // std::cout << "Size of Vector of Vertices: " << sizeof(Vertex) << " * " << vertices.size() << " = " << sizeof(Vertex) * vertices.size() << std::endl;
    // std::cout << "Size of Vector of Indices: " << sizeof(unsigned int) << " * " << mNumIndices << " = " << sizeof(unsigned int) * mNumIndices << std::endl;

    // This is the fix. apparently
    glEnableVertexAttribArray(0);   // "vert"
    glEnableVertexAttribArray(1);   // "vertTexCoord"
    glEnableVertexAttribArray(2);   // "vertNormal"

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*) sizeof(glm::vec3));
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*) ( sizeof(glm::vec3) + sizeof(glm::vec2)) );

    // glBindVertexArray(0);
    // std::cout << "Initialized Vertex Buffer: " << mVertexBuffer << std::endl;
    // std::cout << "Initialized Index Buffer: " << mIndexBuffer << std::endl;

    return true;
}

Mesh::Mesh(){}

Mesh::~Mesh() {
    Clear();
}

void Mesh::Clear() {
    for (unsigned int i = 0; i < mTextures.size(); ++i) {
        delete mTextures.at(i);
    }
}

bool Mesh::LoadMesh(const string& filename) {
    // Clear just in case;
    Clear();

    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(filename.c_str(), 
        aiProcess_Triangulate |
        aiProcess_GenSmoothNormals |
        aiProcess_FlipUVs );

    if (scene) {
        return InitFromScene(scene, filename);
    } 

    cout << "Error parsing '" << filename << ": " << importer.GetErrorString() << endl;
    return false;
}

bool Mesh::InitFromScene(const aiScene* scene, const string& filename) {
    mEntries.resize(scene->mNumMeshes);
    mTextures.resize(scene->mNumMaterials);

    // Initialize the meshes in the scene one by one
    for (unsigned int i = 0; i < mEntries.size(); ++i) {
        const aiMesh* mesh = scene->mMeshes[i];
        InitMesh(i, mesh);
    }

    return InitMaterials(scene, filename);
}

void Mesh::InitMesh(unsigned int index, const aiMesh* mesh) {
    mEntries[index].mMaterialIndex = mesh->mMaterialIndex;

    vector<Vertex> vertices;
    vector<unsigned int> indices;

    const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

    // cout << "Buffer Size: " << mesh->mNumVertices << endl;
    // cout << "Indices Size: " << mesh->mNumFaces*3 << endl;

    for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
        const aiVector3D* pos      = &(mesh->mVertices[i]);
        const aiVector3D* normal   = &(mesh->mNormals[i]);
        const aiVector3D* texCoord = mesh->HasTextureCoords(0) ? &(mesh->mTextureCoords[0][i]) : &Zero3D;

        cout << "Vert: ( " << pos->x << " , " << pos->y << " , " << pos->z << " ) " << endl;
        cout << "Normal: ( " << pos->x << " , " << pos->y << " , " << pos->z << " ) " << endl;
        cout << "texCoord: ( " << texCoord->x << " , " << texCoord->y << " ) " << endl;

        Vertex v(glm::vec3(pos->x, pos->y, pos->z),
                 glm::vec2(texCoord->x, texCoord->y),
                 glm::vec3(normal->x, normal->y, normal->z));

        vertices.push_back(v);
    }

    // cout << "Num Of Vertices = " << vertices.size() << std::endl;

    for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
        const aiFace& face = mesh->mFaces[i];
        assert(face.mNumIndices == 3); // After being triangulated
        indices.push_back(face.mIndices[0]);
        indices.push_back(face.mIndices[1]);
        indices.push_back(face.mIndices[2]);            
    }

    mEntries[index].Init(vertices, indices);
}



bool Mesh::InitMaterials(const aiScene* scene, const string& filename)
{
    // Extract the directory part from the file name
    std::string::size_type SlashIndex = filename.find_last_of("/");
    string Dir;

    if (SlashIndex == string::npos) {
        Dir = ".";
    }
    else if (SlashIndex == 0) {
        Dir = "/";
    }
    else {
        Dir = filename.substr(0, SlashIndex);
    }

    bool Ret = true;

    // Initialize the materials
    for (unsigned int i = 0 ; i < scene->mNumMaterials ; i++) {
        const aiMaterial* material = scene->mMaterials[i];

        mTextures[i] = NULL;

        if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
            aiString path;

            if (material->GetTexture(aiTextureType_DIFFUSE, 0, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
                string fullPath = Dir + "/" + path.data;
                mTextures[i] = new Texture(GL_TEXTURE_2D, fullPath.c_str());

                if (!mTextures[i]->Load()) {
                    printf("Error loading texture '%s'\n", fullPath.c_str());
                    delete mTextures[i];
                    mTextures[i] = NULL;
                    Ret = false;
                }
                else {
                    printf("Loaded texture '%s'\n", fullPath.c_str());
                }
            }
        }
    }

    return Ret;
}

void Mesh::Render(std::shared_ptr<Program> shader)
{
    assert(shader->IsInUse());
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    // cout << "mEntries = " << mEntries.size() << endl;

    for (unsigned int i = 0 ; i < mEntries.size() ; i++) {

        glBindBuffer(GL_ARRAY_BUFFER, mEntries[i].mVertexBuffer);
        glBindVertexArray(mEntries[i].mVertexArray);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEntries[i].mIndexBuffer);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)12);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)20);


        // cout << "Index buffer: " << mEntries[i].mIndexBuffer << std::endl;
        // cout << "Vertex buffer: " << mEntries[i].mVertexBuffer << std::endl;
        // cout << "Num of Indicies: " << mEntries[i].mNumIndices << std::endl;
        const unsigned int materialIndex = mEntries[i].mMaterialIndex;

        if (materialIndex < mTextures.size() && mTextures[materialIndex]) {
            cout << "I'm binding a texture in Mesh::Render() " << endl;
            mTextures[materialIndex]->Bind(shader);
        }

        glDrawElements(GL_TRIANGLES, mEntries[i].mNumIndices, GL_UNSIGNED_INT, NULL);
    }

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
}

void Mesh::SetMaterialData(const aiMaterial* material, Texture* texture) {
    glm::vec4 diffuse = glm::vec4(0.8f,0.8f,0.8f,1.0f);
    glm::vec4 ambient = glm::vec4(0.2f,0.2f,0.2f,1.0f);
    glm::vec4 specular = glm::vec4(0.0f,0.0f,0.0f,1.0f);
    // glm::vec4 emission = glm::vec4(0.0f,0.0f,0.0f,1.0f);

    aiColor4D aiDiffuse; 
    aiColor4D aiAmbient;
    aiColor4D aiSpecular;
    // aiColor4D aiEmission;

    if (aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &aiDiffuse)) {
        color4_to_float4(&aiDiffuse, diffuse);
    }
    texture->SetDiffuse(diffuse);

    if (aiGetMaterialColor(material, AI_MATKEY_COLOR_AMBIENT, &aiAmbient)) {
        color4_to_float4(&aiAmbient, ambient);
    }
    texture->SetAmbient(ambient);

    if (aiGetMaterialColor(material, AI_MATKEY_COLOR_SPECULAR, &aiSpecular)) {
        color4_to_float4(&aiSpecular, specular);
    }
    texture->SetSpecular(specular);

    // if (aiGetMaterialColor(material, AI_MATKEY_COLOR_EMISSIVE, &aiEmission)) {
    //     color4_to_float4(&aiEmission, emission);
    // }
    // texture->SetEmission(emission);

    float shininess = 0.0;
    unsigned int max;
    aiGetMaterialFloatArray(material, AI_MATKEY_SHININESS, &shininess, &max);
    texture->SetShininess(shininess);        
}