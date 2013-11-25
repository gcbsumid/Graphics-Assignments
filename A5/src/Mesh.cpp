// GL Library
#include <GL/glew.h>

// Standard Library
#include <stdexcept>
#include <iostream>
#include <cassert>

// Backlash Library
#include "Mesh.hpp"
#include <assimp/scene.h>
#include <assimp/Importer.hpp> 
#include <assimp/postprocess.h>

using namespace std;

/******************************* WE_Vertex *******************************/

void WE_Vertex::GetFaces(std::vector<WE_Face*>& faces) {
    for (auto& edge : mEdges) {
        if (edge->mVert1 == this && edge->mFaceA) {
            faces.push_back(edge->mFaceA);
        } else if (edge->mVert2 == this && edge->mFaceB) {
            faces.push_back(edge->mFaceB);
        }
    }
}

/********************************* WE_Face *******************************/

void WE_Face::GetVertices(std::vector<WE_Vertex*>& vertices) {
    for (auto& edge : mEdges) {
        if (edge->mFaceA == this) {
            vertices.push_back(edge->mVert1);
        } else if (edge->mFaceB == this) {
            vertices.push_back(edge->mVert2);
        }
    }
}

/********************************** Mesh *********************************/


static void color4_to_float4 (const aiColor4D *c, glm::vec4& f) {
    f = glm::vec4(c->r, c->g, c->b, c->a);
}

Mesh::MeshEntry::MeshEntry()
    : mNumIndices(0)
    , mMaterialIndex(INVALID_MATERIAL)
    , mBaseVertex(0)
    , mBaseIndex(0) 
{
}

Mesh::MeshEntry::~MeshEntry() {

}

Mesh::Mesh(){
    for (int i = 0; i < MAX_BUFFERS; ++i) {
        mBuffers[i] = INVALID_OGL_VALUE;
    }
}

Mesh::~Mesh() {
    Clear();
}

void Mesh::Clear() {
    for (int i = 0; i < MAX_BUFFERS; ++i) {
        if (mBuffers[i] != INVALID_OGL_VALUE) {
            glDeleteBuffers(1, &mBuffers[i]);
        }
    }
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
        aiProcess_FlipUVs | 
        aiProcess_JoinIdenticalVertices );

    if (scene) {
        return InitFromScene(scene, filename);
    } 

    cout << "Error parsing '" << filename << ": " << importer.GetErrorString() << endl;
    return false;
}

bool Mesh::InitFromScene(const aiScene* scene, const string& filename) {
    mEntries.resize(scene->mNumMeshes);
    mTextures.resize(scene->mNumMaterials);
    vector<unsigned int> indices;
    vector<glm::vec3> positions;
    vector<glm::vec3> normals;
    vector<glm::vec2> texcoords;

    // Initialize the meshes in the scene one by one
    unsigned int totalNumOfVertices = 0;
    unsigned int totalNumOfIndices = 0;
    for (unsigned int i = 0; i < mEntries.size(); ++i) {
        const aiMesh* mesh = scene->mMeshes[i];
        InitMesh(i, mesh, positions, normals, texcoords, indices);
        mEntries.at(i).mBaseVertex = totalNumOfVertices;
        mEntries.at(i).mBaseIndex = totalNumOfIndices;

        assert(mEntries.at(i).mVertices.size() == scene->mMeshes[i]->mNumVertices);
        totalNumOfVertices += scene->mMeshes[i]->mNumVertices;
        totalNumOfIndices += mEntries.at(i).mNumIndices;    
    }

    // Set Materials
    if (!InitMaterials(scene, filename)) {
        return false;
    }

    return InitOpenGLData(indices, positions, normals, texcoords);
}

bool Mesh::InitOpenGLData(std::vector<unsigned int>& indices, 
                          std::vector<glm::vec3>& positions,
                          std::vector<glm::vec3>& normals,
                          std::vector<glm::vec2>& texcoords) {
    assert(positions.size() == normals.size() && normals.size() == texcoords.size());

    // Generate Vertex Array  
    if (mVertexArray != INVALID_OGL_VALUE) {
        glDeleteVertexArrays(1, &mVertexArray);
    }
    glGenVertexArrays(1, &mVertexArray);
    glBindVertexArray(mVertexArray);

    // Generate buffers for the indices
    if (mBuffers[INDEX_BUFFER] != INVALID_OGL_VALUE) {
        glDeleteBuffers(1, &mBuffers[INDEX_BUFFER]);
    } 
    glGenBuffers(1, &mBuffers[INDEX_BUFFER]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mBuffers[INDEX_BUFFER]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices[0], GL_STATIC_DRAW);

    // Generate buffers for positions
    if (mBuffers[POSITION_VB] != INVALID_OGL_VALUE) {
        glDeleteBuffers(1, &mBuffers[POSITION_VB]);
    }
    glGenBuffers(1, &mBuffers[POSITION_VB]);
    glBindBuffer(GL_ARRAY_BUFFER, mBuffers[POSITION_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)* positions.size(), &positions[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(POSITION_LOCATION);
    glVertexAttribPointer(POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);    

    // Generate buffers for normals
    if (mBuffers[NORMAL_VB] != INVALID_OGL_VALUE) {
        glDeleteBuffers(1, &mBuffers[NORMAL_VB]);
    }
    glGenBuffers(1, &mBuffers[NORMAL_VB]);
    glBindBuffer(GL_ARRAY_BUFFER, mBuffers[NORMAL_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)* normals.size(), &normals[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(NORMAL_LOCATION);
    glVertexAttribPointer(NORMAL_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);    

    // Generate buffers for texcoords
    if (mBuffers[TEXCOORD_VB] != INVALID_OGL_VALUE) {
        glDeleteBuffers(1, &mBuffers[TEXCOORD_VB]);
    }
    glGenBuffers(1, &mBuffers[TEXCOORD_VB]);
    glBindBuffer(GL_ARRAY_BUFFER, mBuffers[TEXCOORD_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2)* texcoords.size(), &texcoords[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(TEXCOORD_LOCATION);
    glVertexAttribPointer(TEXCOORD_LOCATION, 2, GL_FLOAT, GL_FALSE, 0, 0);    

    return (glGetError() == GL_NO_ERROR);

}


void Mesh::InitMesh(unsigned int index, 
                    const aiMesh* mesh, 
                    vector<glm::vec3>& position,
                    vector<glm::vec3>& normal,
                    vector<glm::vec2>& texcoord,
                    vector<unsigned int>& indices) {
    mEntries[index].mMaterialIndex = mesh->mMaterialIndex;

    const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

    // cout << "Buffer Size: " << mesh->mNumVertices << endl;
    // cout << "Indices Size: " << mesh->mNumFaces*3 << endl;

    // Set mVertices for the mesh
    for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
        const aiVector3D* v_pos      = &(mesh->mVertices[i]);
        const aiVector3D* v_normal   = &(mesh->mNormals[i]);
        const aiVector3D* v_texCoord = mesh->HasTextureCoords(0) ? &(mesh->mTextureCoords[0][i]) : &Zero3D;
        
        WE_Vertex* v = new WE_Vertex();
        v->mPosition = glm::vec3(v_pos->x, v_pos->y, v_pos->z);
        v->mAvgNormal = glm::vec3(v_normal->x, v_normal->y, v_normal->z);
        v->mTexCoord = glm::vec2(v_texCoord->x, v_texCoord->y);
        v->mIndex = i;
        mEntries[index].mVertices.push_back(v);

        position.push_back(v->mPosition);
        normal.push_back(v->mAvgNormal);
        texcoord.push_back(v->mTexCoord);
    }

    // Set mFaces and mEdges for the mesh
    for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
        const aiFace& face = mesh->mFaces[i];
        assert(face.mNumIndices == 3); // After being triangulated

        WE_Face* we_face = new WE_Face();
        mEntries[index].mFaces.push_back(we_face);

        // Creating the Edges
        WE_Edge* prev = nullptr;
        for (unsigned int j = 0; j < face.mNumIndices; ++j) {
            WE_Vertex* vert1 = mEntries[index].mVertices.at(face.mIndices[j]);
            WE_Vertex* vert2 = mEntries[index].mVertices.at(face.mIndices[(j+1)%face.mNumIndices]);

            // pushing vertex 1 onto the indices ~ this will always be true
            indices.push_back(vert1->mIndex);

            // Determine whether the edge already exists
            WE_Edge* edge = NULL;
            for (unsigned int k = 0; k < vert1->mEdges.size(); k++) {
                if (vert1 == vert1->mEdges.at(k)->mVert2) {
                    edge = vert1->mEdges.at(k);
                    break;
                }
            }

            if (edge) {
                // Check that the edge will only every appear twice
                if (edge->mFaceA && edge->mFaceB) {
                    throw runtime_error("Error: This edge is already used twice.");
                }

                // If it does exist, this means that this face is B
                edge->mFaceB = we_face;

                if (prev) {
                    // Set the next and prev if it's not the first edge
                    prev->mNextB = edge;
                    edge->mPrevB = prev;
                } 
            } else {
                // If it doesn't, this means that this face is A 
                // and we have to create the edge
                edge = new WE_Edge();
                mEntries[index].mEdges.push_back(edge);

                // Set the vertices
                edge->mVert1 = vert1;
                edge->mVert2 = vert2;

                // Set the face
                edge->mFaceA = we_face;

                if (prev) {
                    // Set the next and prev if it's not the first edge
                    prev->mNextA = edge;
                    edge->mPrevA = prev;
                }

                // Push the edge onto the vertex's edge list
                vert1->mEdges.push_back(edge);
                vert2->mEdges.push_back(edge);
            }
            prev = edge;

            // Push the edge onto the face's edge list
            we_face->mEdges.push_back(edge);
        }

        // Set the prev of the first edge 
        if (we_face->mEdges.at(0)->mPrevA == nullptr) {
            we_face->mEdges.at(0)->mPrevA = we_face->mEdges.at(face.mNumIndices-1);
        } else {
            we_face->mEdges.at(0)->mPrevB = we_face->mEdges.at(face.mNumIndices-1);
        }

        // Set the next of the last edge
        if (we_face->mEdges.at(face.mNumIndices-1)->mNextA == nullptr) {
            we_face->mEdges.at(face.mNumIndices-1)->mNextA = we_face->mEdges.at(0);
        } else {
            we_face->mEdges.at(face.mNumIndices-1)->mNextB = we_face->mEdges.at(0);
        }         
    }

    // TODO: Change this to account for 4 vertices?
    mEntries.at(index).mNumIndices = mesh->mNumFaces*3;
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
    // TODO: Fix Render
    assert(shader->IsInUse());
    // cout << "mEntries = " << mEntries.size() << endl;

    
    glBindVertexArray(mVertexArray);
    for (unsigned int i = 0 ; i < mEntries.size() ; i++) {
        // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEntries[i].mBuffers[INDEX_BUFFER]);
        // cout << "Index buffer: " << mEntries[i].mIndexBuffer << std::endl;
        // cout << "Vertex buffer: " << mEntries[i].mVertexBuffer << std::endl;
        // cout << "Num of Indicies: " << mEntries[i].mNumIndices << std::endl;
        const unsigned int materialIndex = mEntries[i].mMaterialIndex;

        if (materialIndex < mTextures.size() && mTextures[materialIndex]) {
            // cout << "I'm binding a texture in Mesh::Render() " << endl;
            mTextures[materialIndex]->Bind(shader);
        }

        glDrawElementsInstancedBaseVertex(GL_TRIANGLES,
                                          mEntries.at(i).mNumIndices,
                                          GL_UNSIGNED_INT,
                                          (void*)(sizeof(unsigned int) * mEntries.at(i).mBaseIndex),
                                          1, // TODO: account for multiple instances
                                          mEntries.at(i).mBaseVertex);

        // glDrawElements(GL_TRIANGLES, mEntries[i].mNumIndices, GL_UNSIGNED_INT, NULL);
    }

    glBindVertexArray(0);
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