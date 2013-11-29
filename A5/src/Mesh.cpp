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

    // cout << endl;
    for (auto& edge : mEdges) {
        if (edge->mVert1 == this && edge->mFaceA) {
            faces.push_back(edge->mFaceA);
            // cout << mIndex << ": " << edge->mFaceA->mFaceNormal.x << ", " << edge->mFaceA->mFaceNormal.y << ", " << edge->mFaceA->mFaceNormal.z << endl;
        } else if (edge->mVert2 == this && edge->mFaceB) {
            faces.push_back(edge->mFaceB);
            // cout << mIndex << ": " << edge->mFaceB->mFaceNormal.x << ", "<< edge->mFaceB->mFaceNormal.y << ", "<< edge->mFaceB->mFaceNormal.z << endl;
        } else {
            // cout << mIndex << endl;
        }
    }
}

bool WE_Vertex::IsOnBorder() {
    for (auto& edge : mEdges) {
        if (edge->IsOnBorder()) {
            return true;
        }
    }
    return false;
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

/********************************* WE_Edge *******************************/

bool WE_Edge::IsOnBorder() {
    if (!mFaceA || !mFaceB) {
        return true;
    }
    return false;
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

    if (mBuffers[0] != INVALID_OGL_VALUE) {
        glDeleteBuffers(6, mBuffers);
    }
    if (mVertexArray != INVALID_OGL_VALUE) {
        glDeleteVertexArrays(1, &mVertexArray);
    }

    for (unsigned int i = 0; i < mTextures.size(); ++i) {
        delete mTextures.at(i);
    }

}

bool Mesh::LoadMesh(const string& filename) {
    // Clear just in case;
    Clear();

    Assimp::Importer importer;
    std::cout << "Loading: " << filename << endl;

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

    cout << "number of meshes: " << scene->mNumMeshes << endl;

    // Initialize the meshes in the scene one by one
    unsigned int totalNumOfVertices = 0;
    unsigned int totalNumOfIndices = 0;
    for (unsigned int i = 0; i < mEntries.size(); ++i) {
        const aiMesh* mesh = scene->mMeshes[i];
        cout << "Name of Mesh: " << mesh->mName.C_Str() << endl;
        cout << "Number of Vertices: " << mesh->mNumVertices << endl;
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
    // cerr << "I'm returning false here!" << endl;

    return InitOpenGLData(indices, positions, normals, texcoords);
}

bool Mesh::InitOpenGLData(std::vector<unsigned int>& indices, 
                          std::vector<glm::vec3>& positions,
                          std::vector<glm::vec3>& normals,
                          std::vector<glm::vec2>& texcoords) {
    assert(positions.size() == normals.size() && normals.size() == texcoords.size());

    Clear();

    glGenBuffers(5, mBuffers);
    // Generate Vertex Array  
    glGenVertexArrays(1, &mVertexArray);
    glBindVertexArray(mVertexArray);

    // Generate buffers for the indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mBuffers[INDEX_BUFFER]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices[0], GL_STATIC_DRAW);

    // Generate buffers for positions
    glBindBuffer(GL_ARRAY_BUFFER, mBuffers[POSITION_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)* positions.size(), &positions[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(POSITION_LOCATION);
    glVertexAttribPointer(POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);    

    // Generate buffers for normals
    // for(auto& normal : normals) {
    //     cout << "Normal: " << normal.x << ", " << normal.y << ", " << normal.z << endl;
    // }
    glBindBuffer(GL_ARRAY_BUFFER, mBuffers[NORMAL_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)* normals.size(), &normals[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(NORMAL_LOCATION);
    glVertexAttribPointer(NORMAL_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);    

    // Generate buffers for texcoords
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
    mEntries.at(index).mMaterialIndex = mesh->mMaterialIndex;

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
        mEntries.at(index).mVertices.push_back(v);

        position.push_back(v->mPosition);
        normal.push_back(v->mAvgNormal);
        texcoord.push_back(v->mTexCoord);

        cout << "Vertex Normal: " << v_normal->x << ", " << v_normal->y  << ", " << v_normal->z << endl;
    }

    // cout << "Next Mesh: " << endl << endl;

    // Set mFaces and mEdges for the mesh
    for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
        const aiFace& face = mesh->mFaces[i];
        // cout << face.mNumIndices << endl;
        assert(face.mNumIndices == 3); // After being triangulated

        vector<WE_Vertex*> face_vertices;
        face_vertices.push_back(mEntries.at(index).mVertices.at(face.mIndices[0]));
        face_vertices.push_back(mEntries.at(index).mVertices.at(face.mIndices[1]));
        face_vertices.push_back(mEntries.at(index).mVertices.at(face.mIndices[2]));

        CreateFace(mEntries[index].mEdges,
                   mEntries[index].mFaces,
                   face_vertices,
                   indices);
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
    cout << "number of materials: " << scene->mNumMaterials << endl;

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
            } else {
                cout << "No Texture!" << endl;
            }
        } else { 
            cout << "No Texture at all!" << endl;
        }
    }

    return Ret;
}

void Mesh::AttachColor(glm::vec3 color, unsigned int meshID) {
    mEntries.at(meshID).mColor = color;
}

void Mesh::Render(std::shared_ptr<Program>& shader)
{
    // TODO: Fix Render
    assert(shader->IsInUse());
    
    glBindVertexArray(mVertexArray);
    for (unsigned int i = 0 ; i < mEntries.size() ; i++) {
        const unsigned int materialIndex = mEntries[i].mMaterialIndex;

        if (materialIndex < mTextures.size() && mTextures[materialIndex]) {
            mTextures[materialIndex]->Bind(shader);
        }

        if (mEntries.at(i).mColor != glm::vec3()) {
            glUniform3f(shader->Uniform("mat_color"), mEntries.at(i).mColor.x,
                mEntries.at(i).mColor.y, mEntries.at(i).mColor.z);
        } 

        glDrawElementsInstancedBaseVertex(GL_TRIANGLES,
                                          mEntries.at(i).mNumIndices,
                                          GL_UNSIGNED_INT,
                                          (void*)(sizeof(unsigned int) * mEntries.at(i).mBaseIndex),
                                          1, // TODO: account for multiple instances
                                          mEntries.at(i).mBaseVertex);

        if (mEntries.at(i).mColor != glm::vec3()) {
            glUniform3f(shader->Uniform("mat_color"), -1.0f, -1.0f, -1.0f);
        }
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

void Mesh::Subdivide(int numOfIteration) {
    // Todo:
    //      - subdivde X times
    //      - Re-initialize the OpenGL data
    numOfIteration -= 1;

    vector<glm::vec3> positions;
    vector<glm::vec3> normals;
    vector<glm::vec2> texcoords;
    vector<unsigned int> indices;
    unsigned int totalNumOfVertices = 0;
    unsigned int prevTotalNumOfIndices = 0;

    for (unsigned int i = 0; i < mEntries.size(); ++i) {
        SubdivideMeshEntry(mEntries.at(i).mEdges, mEntries.at(i).mFaces, mEntries.at(i).mVertices, indices);

        // Adding the positions, normals, and texcoords 
        // Setting stuff for the opengl data
        if (numOfIteration == 0) {
            for (auto& vert : mEntries.at(i).mVertices) {
                GetVertexNormal(vert, normals);
                positions.push_back(vert->mPosition);
                texcoords.push_back(vert->mTexCoord);
            }

            mEntries.at(i).mBaseVertex = totalNumOfVertices;
            mEntries.at(i).mBaseIndex = indices.size();
            mEntries.at(i).mNumIndices = indices.size() - prevTotalNumOfIndices;

            totalNumOfVertices += mEntries.at(i).mVertices.size();
            prevTotalNumOfIndices = indices.size();
        } else {
            indices.clear(); // We don't want to keep the indices if we're not going to display it to opengl
        }
    }

    if (numOfIteration == 0) {
        // Initializing OpenGL
        InitOpenGLData(indices, positions, normals, texcoords);
    } else {
        // re-iterate
        Subdivide(numOfIteration);
    }
}

void Mesh::SubdivideMeshEntry(vector<WE_Edge*>& edge_list,
                              vector<WE_Face*>& face_list,
                              vector<WE_Vertex*>& vertex_list,
                              vector<unsigned int>& indices) {
    unsigned int total_vertices_size = vertex_list.size();
    unsigned int original_size = total_vertices_size;

    // calculate face points
    for (auto& face : face_list) {
        vector<WE_Vertex*> vertices;
        face->GetVertices(vertices);

        glm::vec3 avg_pos;
        for (auto& vert : vertices) {
            avg_pos += vert->mPosition;
        }
        WE_Vertex* face_point = new WE_Vertex();
        face_point->mIndex = total_vertices_size++;
        vertex_list.push_back(face_point);

        face_point->mPosition = avg_pos / 3.0f;
        face->mFaceVertex = face_point;
    }

    // calculate edge points
    for (auto& edge : edge_list) {
        WE_Vertex* edge_point = new WE_Vertex();
        edge_point->mIndex = total_vertices_size++;
        vertex_list.push_back(edge_point);
        edge_point->mPosition = (edge->mVert1->mPosition + edge->mVert2->mPosition) * 0.5f;

        // if not a border edge
        if (edge->mFaceA && edge->mFaceB) {
            assert(edge->mFaceA->mFaceVertex && edge->mFaceB->mFaceVertex);
            glm::vec3 face_avg = (edge->mFaceA->mFaceVertex->mPosition + edge->mFaceB->mFaceVertex->mPosition) * 0.5f;
            edge_point->mPosition = (edge_point->mPosition + face_avg) * 0.5f;
        }

        edge->mMidPointVertex = edge_point;
    }

    // calculate vertex updates
    for (unsigned int i = 0; i < original_size; ++i) {
    // for (auto& vertex : vertex_list) {
        WE_Vertex* vertex = vertex_list.at(i);
        glm::vec3 old_coords = vertex->mPosition;

        if (vertex->IsOnBorder()) {
            // get all the border edges
            vector<WE_Edge*> border_edges;
            for (auto& edge : vertex->mEdges) {
                if (edge->IsOnBorder()) {
                    border_edges.push_back(edge);
                }
            }

            // add all the border edge midpoints
            vertex->mPosition = glm::vec3();
            for (auto& edge : border_edges) {
                assert(edge->mMidPointVertex);
                vertex->mPosition += edge->mMidPointVertex->mPosition;
            }
            // add the old coordinate
            vertex->mPosition += old_coords;

            // Average the shit out
            vertex->mPosition = (vertex->mPosition) / ((float)border_edges.size()+1);
        } else {
            vector<WE_Face*> faces;

            // get the average face point
            vertex->GetFaces(faces);
            float numOfFaces = (double)faces.size();
            glm::vec3 avg_face_point;
            for (auto& face : faces) {
                assert(face->mFaceVertex);
                avg_face_point += face->mFaceVertex->mPosition;
            }
            avg_face_point = avg_face_point /((float)faces.size());

            // get the average edge points among the magestic edges
            glm::vec3 avg_edge_point;
            for (auto& edge : vertex->mEdges) {
                assert(edge->mMidPointVertex);
                avg_edge_point += edge->mMidPointVertex->mPosition;
            }
            avg_edge_point = avg_edge_point / ((float)vertex->mEdges.size());

            // get the updated point from these smart men named Catmull and Clark
            vertex->mPosition = old_coords * ((numOfFaces-3)/numOfFaces);
            vertex->mPosition += avg_face_point * (1.0f / numOfFaces);
            vertex->mPosition += avg_edge_point * (2.0f / numOfFaces);
        }
    }

    // delete all the current mEdges attached to all vertices
    for (unsigned int i = 0; i < original_size; ++i) {
        WE_Vertex* vertex = vertex_list.at(i);

        vertex->mEdges.clear();
    }

    // cout << "vertex_list size: " << vertex_list.size() << endl;

    vector<WE_Edge*> new_edges;
    vector<WE_Face*> new_faces;
    // Now I have all the points that I need. I should do something with it.
    for (auto& face : face_list) {
        for (auto& edge : face->mEdges) {

            vector<WE_Vertex*> face_vertices;
            if (edge->mFaceA == face) {
                face_vertices.push_back(edge->mMidPointVertex);
                face_vertices.push_back(face->mFaceVertex);
                face_vertices.push_back(edge->mVert1);
                
                CreateFace(new_edges,
                           new_faces,
                           face_vertices, 
                           indices);

                face_vertices.clear();

                face_vertices.push_back(edge->mVert2);
                face_vertices.push_back(face->mFaceVertex);
                face_vertices.push_back(edge->mMidPointVertex);

                CreateFace(new_edges,
                           new_faces,
                           face_vertices,
                           indices);
            } else if (edge->mFaceB == face) {
                face_vertices.push_back(face->mFaceVertex);
                face_vertices.push_back(edge->mMidPointVertex);
                face_vertices.push_back(edge->mVert1);

                CreateFace(new_edges,
                           new_faces,
                           face_vertices,
                           indices);

                face_vertices.clear();

                face_vertices.push_back(edge->mVert2);
                face_vertices.push_back(edge->mMidPointVertex);
                face_vertices.push_back(face->mFaceVertex);

                CreateFace(new_edges,
                           new_faces,
                           face_vertices,
                           indices);
            } else {
                throw runtime_error("Creating a new face. not A or B.");
            }
        }
    }
    // cerr << "I exit after here!" << endl;

    // removing the old faces and edges
    for (auto& face : face_list) {
        delete face;
    }

    for (auto& edge : edge_list) {
        delete edge;
    }

    // saving the new data to the mesh
    face_list = new_faces;
    edge_list = new_edges;
}

void Mesh::CreateFace(vector<WE_Edge*>& edges_list, 
                      vector<WE_Face*>& face_list, 
                      vector<WE_Vertex*>& face_vertices,
                      vector<unsigned int>& indices) {
    // Creating the Edges
    WE_Face* face = new WE_Face();
    face_list.push_back(face);

    WE_Edge* prev = nullptr;
    for (unsigned int j = 0; j < 3; ++j) {
        WE_Vertex* vert1 = face_vertices.at(j);
        WE_Vertex* vert2 = face_vertices.at((j+1)%3);
        // pushing vertex 1 onto the indices ~ this will always be true
        indices.push_back(vert1->mIndex);

        // Determine whether the edge already exists
        WE_Edge* edge = NULL;
        for (unsigned int k = 0; k < vert2->mEdges.size(); k++) {
            if (vert1 == vert2->mEdges.at(k)->mVert1 ||
                vert1 == vert2->mEdges.at(k)->mVert2) {
                edge = vert2->mEdges.at(k);
                break;
            }
        }

        if (edge) {
            // Check that the edge will only every appear twice
            if (edge->mFaceA && edge->mFaceB) {
                throw runtime_error("Error: This edge is already used twice.");
            }

            edge->mFaceB = face;

            if (prev) {
                // Set the next and prev if it's not the first edge
                if (prev->mFaceA == face) {
                    prev->mNextA = edge;
                } else {
                    prev->mNextB = edge;
                }
                edge->mPrevB = prev;
            } 
        } else {
            // If it doesn't, this means that this face is A 
            // and we have to create the edge
            edge = new WE_Edge();
            edges_list.push_back(edge);

            // Set the vertices
            edge->mVert1 = vert1;
            edge->mVert2 = vert2;

            // Set the face
            edge->mFaceA = face;

            if (prev) {
                // Set the next and prev if it's not the first edge
                if (prev->mFaceA == face) {
                    prev->mNextA = edge;                        
                } else {
                    prev->mNextB = edge;
                }
                edge->mPrevA = prev;
            }

            // Push the edge onto the vertex's edge list
            vert1->mEdges.push_back(edge);
            vert2->mEdges.push_back(edge);
        }
        prev = edge;

        // Push the edge onto the face's edge list
        face->mEdges.push_back(edge);
    }

    // Set the prev of the first edge 
    if (face->mEdges.at(0)->mPrevA == nullptr) {
        face->mEdges.at(0)->mPrevA = face->mEdges.at(2);
    } else {
        face->mEdges.at(0)->mPrevB = face->mEdges.at(2);
    }

    // Set the next of the last edge
    if (face->mEdges.at(2)->mNextA == nullptr) {
        face->mEdges.at(2)->mNextA = face->mEdges.at(0);
    } else {
        face->mEdges.at(2)->mNextB = face->mEdges.at(0);
    }

    // cout << "mEdges size: " << face->mEdges.size() << endl;

    glm::vec3 firstEdgeVector = face_vertices.at(1)->mPosition - face_vertices.at(0)->mPosition; 
    glm::vec3 lastEdgeVector = face_vertices.at(2)->mPosition - face_vertices.at(0)->mPosition;
    // cout << "firstEdgeVector: " << firstEdgeVector.x << ", " << firstEdgeVector.y << ", " << firstEdgeVector.z << endl;
    // cout << "lastEdgeVector: " << lastEdgeVector.x << ", " << lastEdgeVector.y << ", " << lastEdgeVector.z << endl;
    face->mFaceNormal = glm::cross(firstEdgeVector, lastEdgeVector);
    // glm::vec3 temp = glm::normalize(face->mFaceNormal);
    // cout << "Normal: " << temp.x << ", " << temp.y << ", " << temp.z << endl;
}

void Mesh::GetVertexNormal(WE_Vertex* vert, vector<glm::vec3>& normals) {
    vector<WE_Face*> facesAttachedToVertex;
    vert->GetFaces(facesAttachedToVertex);

    glm::vec3 norm;
    // cout << "Index: " << vert->mIndex << endl;
    // cout << "Faces attached size: " << facesAttachedToVertex.size() << endl;
    for (auto& face : facesAttachedToVertex) {
        vector<WE_Edge*> edges;
        // cout << "Faces: " << face->mEdges.size() << endl;
        for(auto& edge : face->mEdges) {
            if(edge->mVert1 == vert || edge->mVert2 == vert) {
                edges.push_back(edge);
            }
        }

        if (edges.size() > 2) {
            throw runtime_error("Number of edges is greater than 2.");
        }

        glm::vec3 edge1, edge2;
        if (edges.at(0)->mVert1 == vert) {
            edge1 = edges.at(0)->mVert2->mPosition - edges.at(0)->mVert1->mPosition;
        } else {
            edge1 = edges.at(0)->mVert1->mPosition - edges.at(0)->mVert2->mPosition;
        }
        if (edges.at(1)->mVert1 == vert) {
            edge2 = edges.at(1)->mVert2->mPosition - edges.at(1)->mVert1->mPosition;
        } else {
            edge2 = edges.at(1)->mVert1->mPosition - edges.at(1)->mVert2->mPosition;
        }

        if (glm::length(edge1) && glm::length(edge2)) {
            edge1 = glm::normalize(edge1);
            edge2 = glm::normalize(edge2);
        } else {
            cerr << "edge1: " << glm::length(edge1) << " and edge2: " << glm::length(edge2) << endl;
            throw runtime_error("Invalid Length.");
        }

        float vertexAngle = acos(glm::dot(edge1, edge2));

        // cout << "face normal: " << face->mFaceNormal.x << ", " << face->mFaceNormal.y << ", " << 
        //     face->mFaceNormal.z  << " and " << vertexAngle << endl;
        // cout << vert->mIndex << " face->mFaceNormal: " << (face->mFaceNormal * vertexAngle).x << ", " << (face->mFaceNormal * vertexAngle).y << ", " << (face->mFaceNormal * vertexAngle).z << endl;
        norm += (face->mFaceNormal * vertexAngle);
    }
    // cout << vert->mIndex << " pre-normal: " << norm.x << ", " << norm.y << ", " << norm.z << endl;

    vert->mAvgNormal = glm::normalize(norm);
    // cout << vert->mIndex << " Position: " << vert->mPosition.x << ", " << vert->mPosition.y << ", " << 
    //     vert->mPosition.z << endl;
    // cout << vert->mIndex << " normal: " << vert->mAvgNormal.x << ", " << vert->mAvgNormal.y << ", " << 
    //     vert->mAvgNormal.z << endl;
    normals.push_back(vert->mAvgNormal);
}