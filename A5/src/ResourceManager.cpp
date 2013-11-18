#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include <glm/glm.hpp>

#include <cassert>
#include <iostream>
#include <stdexcept>

#include "ResourceManager.hpp"
#include "Bitmap.hpp"

using namespace std;

static const aiVector3D ZERO_3D(0.0f,0.0f,0.0f);

static void color4_to_float4 (const aiColor4D *c, glm::vec4& f) {
    f = glm::vec4(c->r, c->g, c->b, c->a);
}

ResourceManager::ResourceManager() {

}

ResourceManager::~ResourceManager() {

}

void ResourceManager::SetTextureSharedPtr(shared_ptr<map<string, Texture*>>& textures) {
    mTextures = textures;
}

void ResourceManager::SetMeshSharedPtr(shared_ptr<map<int, shared_ptr<Mesh>>>& meshes) {
    mMeshes = meshes;
}

void ResourceManager::LoadAssetFromFile(const string& file) {
    Assimp::Importer importer;

    // TODO: Look at the aiProcess enum
    const aiScene* scene = importer.ReadFile(file.c_str(),
        aiProcess_GenSmoothNormals |    
        aiProcess_Triangulate |
        aiProcess_JoinIdenticalVertices    
    );

    if (scene) {
        cout << "Number of Materials: " << scene->mNumMaterials << endl;
        InitMaterials(scene, file);
        cout << "Number of Meshes: " << scene->mNumMeshes << endl;
        ProcessScene(scene);
    } else {
        throw runtime_error(importer.GetErrorString());
    }

    importer.FreeScene(); // Hopefully this works
}

void ResourceManager::ProcessScene(const aiScene* scene) {
    for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
        const aiMesh* mesh = scene->mMeshes[i];
        InitMesh(mesh);
    }
}

void ResourceManager::InitMesh(const aiMesh* mesh) {
    assert(mMeshes.use_count());

    shared_ptr<Mesh> meshEntry(new Mesh(mMeshes->size()));
    cout << "Adding Mesh with ID: " << meshEntry->mID << endl;
    meshEntry->mMaterialName = mLocalTexture.at(mesh->mMaterialIndex);

    vector<Vertex> vertices;
    vector<unsigned int> indices;

    cout << "Number of Vertices: " << mesh->mNumVertices << endl;

    for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
        const aiVector3D* pos = &(mesh->mVertices[i]);
        const aiVector3D* normal = &(mesh->mNormals[i]);
        const aiVector3D* texCoord = mesh->HasTextureCoords(0) ? &(mesh->mTextureCoords[0][i]) : &ZERO_3D;

        Vertex v(glm::vec3(pos->x, pos->y, pos->z),
                 glm::vec2(texCoord->x, texCoord->y),
                 glm::vec3(normal->x, normal->y, normal->z));

        // Updating the Bounding Box
        // if (!box.IsPointInside(glmPos)) {
        //     box.Update(glmPos);
        // }

        vertices.push_back(v);
    }        

    for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
        const aiFace& face = mesh->mFaces[i];
        assert(face.mNumIndices == 3); // After being triangulated
        indices.push_back(face.mIndices[0]);
        indices.push_back(face.mIndices[1]);
        indices.push_back(face.mIndices[2]);            
    }

    // cout << "Actual number of Vertices: " << vertices.size() << endl;
    // cout << "Actual number of Indices: " << indices.size() << endl;
    // meshEntry->mName = mesh->mName.C_Str();
    meshEntry->Init(vertices, indices);

    // cout << "Adding Mesh: " << mesh->mName << endl;
    // mMeshes->insert(make_pair(meshEntry->mName, meshEntry));
    mMeshes->insert(make_pair(meshEntry->mID, meshEntry));
}

void ResourceManager::InitMaterials(const aiScene* scene, const string& filename) {
    // TODO: If there is not texture, just keep it null or something. I'll work on it.
    assert(mTextures.use_count());
    mLocalTexture.clear();

    string::size_type slashIndex = filename.find_last_of("/");
    string dir;

    if (slashIndex == string::npos) {
        dir = ".";
    } else if (slashIndex == 0) {
        dir = "/";
    } else {
        dir = filename.substr(0, slashIndex);
    }

    for (unsigned int i = 0; i < scene->mNumMaterials; ++i) {
        const aiMaterial* material = scene->mMaterials[i];

        bool gotTexture = false;
        if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
            aiString path;
            string name;

            if (material->GetTexture(aiTextureType_DIFFUSE, 0, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
                gotTexture = true;
                name = path.data;

                mLocalTexture.push_back(name);
                
                if(mTextures->count(name) == 0) {
                    string fullPath = dir + "/" + name;
                    cout << "Opening file: " << fullPath << endl;
                    Bitmap bmp = Bitmap::BitmapFromFile(fullPath);
                    bmp.FlipVertically();

                    cout << "Adding Texture: " << name << endl;
                    Texture* texture = new Texture(bmp, name);
                    mTextures->insert(make_pair(name, texture));
                    SetMaterialData(material, texture);
                }
            }
        } 

        if (!gotTexture) {
            string name = "INVALID";
            mLocalTexture.push_back(name);
        }
    }
}

void ResourceManager::SetMaterialData(const aiMaterial* material, Texture* texture) {
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