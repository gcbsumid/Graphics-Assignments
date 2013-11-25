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
    , mRand(-1,1)
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
    for (int curX = minX; curX < maxX; ++curX) {
        for (int curZ = minZ; curZ < maxZ; ++curZ) {
            WE_Vertex* v = new WE_Vertex();

            double prevXHeight = (curX == minX) ? 0 : mVertices.at((curX-1)*mGridX + curZ)->mPosition.y;
            double prevYHeight = (curZ == minZ) ? 0 : mVertices.at((curX * mGridX) + (curZ - 1))->mPosition.y;
            curHeight = height((prevXHeight + prevYHeight)/2);

            v->mPosition = glm::vec3((double)curX, curHeight, (double)curZ);
            v->mTexCoord = glm::vec2(0,0);
            v->mIndex = idx++;

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

    // Find the Average Normal for each vertex
    for (auto& vert : mVertices) {
        vector<WE_Face*> facesAttachedToVertex;
        vert->GetFaces(facesAttachedToVertex);

        glm::vec3 norm; 
        for (auto& face : facesAttachedToVertex) {
            norm += face->mFaceNormal;
        }
        vert->mAvgNormal = glm::normalize(norm);
        normals.push_back(vert->mAvgNormal);
    }

    // OpenGL-ize all data
    mNumIndices = indices.size();
    InitOpenGLData(indices, positions, normals, texcoords);
}

void GroundMesh::CreateFace(vector<unsigned int>& face_indices, 
                            vector<unsigned int>& indices) {

    WE_Face* we_face = new WE_Face();
    mFaces.push_back(we_face);

    // Creating the Edges
    WE_Edge* prev = nullptr;
    for (unsigned int j = 0; j < face_indices.size(); ++j) {
        WE_Vertex* vert1 = mVertices.at(face_indices.at(j));
        WE_Vertex* vert2 = mVertices.at(face_indices.at((j+1)%face_indices.size()));

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
    WE_Vertex* vert0 = mVertices.at(0);
    WE_Vertex* vert1 = mVertices.at(1);
    WE_Vertex* vert2 = mVertices.at(2);

    glm::vec3 firstEdgeVector = vert0->mPosition - vert1->mPosition; 
    glm::vec3 lastEdgeVector = vert0->mPosition - vert2->mPosition;

    we_face->mFaceNormal = glm::normalize(glm::cross(firstEdgeVector, lastEdgeVector));

}

GroundMesh::~GroundMesh() {

}

double GroundMesh::height(double currentHeight) {
    // return 0; // Test
    return (currentHeight + mRand(mGen));
}


void GroundMesh::RunFractalAlgorithm(int numRecurse) {
    // TODO: This algorithm
}

void GroundMesh::Render(std::shared_ptr<Program> shader) {
    assert(shader->IsInUse());

    glBindVertexArray(mVertexArray);
    glDrawElementsInstanced(GL_TRIANGLES,
        mNumIndices,
        GL_UNSIGNED_INT,
        (void *)0,
        1);
    glBindVertexArray(0);
}

