#include <GL/glew.h>

#include <stdexcept>
#include <iostream>
#include <cassert>
#include <random>

#include "GroundMesh.hpp"

using namespace std;

GroundMesh::GroundMesh(int gridX,
                       int gridZ,
                       double maxHeight,
                       double minHeight) 
    : mNumIndices(0)
    , mGen(mRD())
    , mRand(-2,2)
    , mGridX(gridX)
    , mGridZ(gridZ)
    , mMinHeight(minHeight)
    , mMaxHeight(maxHeight)
{
    assert(gridX > 0 && gridZ > 0);

    vector<glm::vec3> positions;
    vector<glm::vec3> normals;
    vector<glm::vec2> texcoords;
    vector<unsigned int> indices;

    // TODO: get the normal for each vertex
    double curHeight = 0;
    double idx = 0;

    // Generate the vertices
    int minX = -gridX/2;
    int minZ = -gridZ/2;
    int maxX = gridX/2;
    int maxZ = gridZ/2;

    // int x_idx = 0;
    // int z_idx = 0;
    for (int curX = minX, x_idx = 0; curX < maxX; ++curX, ++x_idx) {
        for (int curZ = minZ, z_idx = 0; curZ < maxZ; ++curZ, ++z_idx) {
            WE_Vertex* v = new WE_Vertex();

            // cout << "x_idx: " << x_idx << endl;
            // cout << "z_idx: " << z_idx << endl;

            // cout << "x: " << ((x_idx == 0) ? 0 : ((x_idx-1)*mGridX + z_idx)) << endl;
            // cout << "z: " << ((z_idx == 0) ? 0 : ((x_idx * mGridX) + (z_idx - 1))) << endl;
            double prevXHeight = (x_idx == 0) ? 0 : mVertices.at((x_idx-1)*mGridX + z_idx)->mPosition.y;
            double prevYHeight = (z_idx == 0) ? 0 : mVertices.at((x_idx * mGridX) + (z_idx - 1))->mPosition.y;
            curHeight = height((prevXHeight + prevYHeight)/2);
            // cout << "Height: " << curHeight << endl;

            v->mPosition = glm::vec3((double)curX/10.0, curHeight, (double)curZ/10.0);
            v->mTexCoord = glm::vec2(0,0);
            v->mIndex = idx++;
            mVertices.push_back(v);

            // cout << v->mIndex << ": " << v->mPosition.x << ", " << v->mPosition.y << ", " << v->mPosition.z << endl;

            positions.push_back(v->mPosition);
            texcoords.push_back(v->mTexCoord);
        }
    }


    // Generate the Faces and Edges
    vector<unsigned int> vertices;
    for (int curX = 0; curX < mGridX - 1; ++curX) {
        for (int curZ = 0; curZ < mGridZ - 1; ++curZ) {
            vertices.clear();
            vertices.push_back(curX*mGridX + curZ);
            vertices.push_back(curX*mGridX + curZ+1);
            vertices.push_back((curX+1)*mGridX + curZ+1);
            CreateFace(vertices, indices);

            vertices.clear();
            vertices.push_back((curX+1)*mGridX + curZ+1);
            vertices.push_back((curX+1)*mGridX + curZ);
            vertices.push_back(curX*mGridX + curZ);
            CreateFace(vertices, indices);
        }
    }
    // cout << "I'm almost done creating Ground!" << endl;

    // Find the Average Normal for each vertex
    for (auto& vert : mVertices) {
        vector<WE_Face*> facesAttachedToVertex;
        vert->GetFaces(facesAttachedToVertex);

        // cout << "size: " << facesAttachedToVertex.size() << endl;

        glm::vec3 norm; 
        for (auto& face : facesAttachedToVertex) {
            norm += face->mFaceNormal;
        }
        vert->mAvgNormal = glm::normalize(norm);
        // cout << "index: " << vert->mIndex << " and normal: " 
        //     << vert->mAvgNormal.x << ", " << vert->mAvgNormal.y << ", " << vert->mAvgNormal.z << endl;
        normals.push_back(vert->mAvgNormal);
    }

    // OpenGL-ize all data
    mNumIndices = indices.size();
    cout << "numIndices: " << mNumIndices << endl;

    // int i = 0;
    // for (auto n : normals) {
    //     // cout << i++ << " Position: " << p.x << ", " << p.y << ", " << p.z << endl;
    //     cout << i++ << " Normals: " << p.x << ", " << p.y << ", " << p.z << endl;
    //     // cout << "index: " << i << endl;
    // }

    InitOpenGLData(indices, positions, normals, texcoords);
}

void GroundMesh::CreateFace(vector<unsigned int>& face_indices, 
                            vector<unsigned int>& indices) {

    WE_Face* we_face = new WE_Face();
    mFaces.push_back(we_face);
    // cout << "face number: " << mFaces.size()-1 << endl;

    // cout << endl;
    // Creating the Edges
    WE_Edge* prev = nullptr;
    for (unsigned int j = 0; j < face_indices.size(); ++j) {
        // std::cout << j << "-th index: " << face_indices.at(j) << endl;
        // std::cout << ((j+1)%face_indices.size()) << "-th index: " << face_indices.at((j+1)%face_indices.size()) << endl;
        WE_Vertex* vert1 = mVertices.at(face_indices.at(j));
        WE_Vertex* vert2 = mVertices.at(face_indices.at((j+1)%face_indices.size()));

        // pushing vertex 1 onto the indices ~ this will always be true
        indices.push_back(vert1->mIndex);
        // std::cout << j << "-th index: " << vert1->mIndex << endl;
        // std::cout << "vertex position: " << vert1->mPosition.x << ", " <<vert1->mPosition.y 
        //           << ", " << vert1->mPosition.z << endl;


        // Determine whether the edge already exists
        WE_Edge* edge = NULL;
        for (unsigned int k = 0; k < vert2->mEdges.size(); k++) {
            // if (vert1 == vert1->mEdges.at(k)->mVert2) {
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
            mEdges.push_back(edge);

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
        we_face->mEdges.at(0)->mPrevA = we_face->mEdges.at(face_indices.size()-1);
    } else {
        we_face->mEdges.at(0)->mPrevB = we_face->mEdges.at(face_indices.size()-1);
    }

    // Set the next of the last edge
    if (we_face->mEdges.at(face_indices.size()-1)->mNextA == nullptr) {
        we_face->mEdges.at(face_indices.size()-1)->mNextA = we_face->mEdges.at(0);
    } else {
        we_face->mEdges.at(face_indices.size()-1)->mNextB = we_face->mEdges.at(0);
    } 

    // Get the normal of the face
    WE_Vertex* vert0 = mVertices.at(face_indices.at(0));
    WE_Vertex* vert1 = mVertices.at(face_indices.at(1));
    WE_Vertex* vert2 = mVertices.at(face_indices.at(2));

    glm::vec3 firstEdgeVector = vert0->mPosition - vert1->mPosition; 
    glm::vec3 lastEdgeVector = vert0->mPosition - vert2->mPosition;

    we_face->mFaceNormal = glm::normalize(glm::cross(firstEdgeVector, lastEdgeVector));

}

GroundMesh::~GroundMesh() {

}

double GroundMesh::height(double currentHeight) {
    // return 0; // Test
    if (currentHeight >= mMaxHeight || currentHeight <= mMinHeight) {
        return currentHeight;
    }
    // std::cout << "mRand(Gen): " << mRand(mGen) << endl;
    return (currentHeight + mRand(mGen));


    // double height = 0;
    // double var = mRand(mGen);
    // if (var > 1.0) {
    //     height = 1.0;
    // } else if (var < -1.0) {
    //     height = -1.0;
    // }

    // return height;
}


void GroundMesh::RunFractalAlgorithm(int numRecurse) {
    // TODO: This algorithm
}

void GroundMesh::Render(std::shared_ptr<Program> shader) {
    assert(shader->IsInUse());

    glBindVertexArray(mVertexArray);
    glDrawElements(GL_TRIANGLES,
        mNumIndices,
        GL_UNSIGNED_INT,
        NULL);
    glBindVertexArray(0);
}

