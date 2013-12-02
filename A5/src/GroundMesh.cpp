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
                       double minHeight,
                       bool withHeight) 
    : Mesh(true)
    , mNumIndices(0)
    , mGen(mRD())
    , mRand(-2,2)
    , mGridX(gridX)
    , mGridZ(gridZ)
    , mMinHeight(minHeight)
    , mMaxHeight(maxHeight)
    , mWithHeight(withHeight)
{
    assert(gridX > 0 && gridZ > 0);

    vector<glm::vec3> positions;
    vector<glm::vec3> normals;
    vector<glm::vec2> texcoords;
    vector<unsigned int> indices;

    double curHeight = 0;
    double idx = 0;

    // Generate the vertices
    int minX = -gridX/2;
    int minZ = -gridZ/2;
    int maxX = gridX/2;
    int maxZ = gridZ/2;

    for (int curX = minX, x_idx = 0; curX < maxX; ++curX, ++x_idx) {
        for (int curZ = minZ, z_idx = 0; curZ < maxZ; ++curZ, ++z_idx) {
            WE_Vertex* v = new WE_Vertex();

            double prevXHeight = (x_idx == 0) ? 0 : mVertices.at((x_idx-1)*mGridX + z_idx)->mPosition.y;
            double prevYHeight = (z_idx == 0) ? 0 : mVertices.at((x_idx * mGridX) + (z_idx - 1))->mPosition.y;
            curHeight = height((prevXHeight + prevYHeight)/2);

            v->mPosition = glm::vec3((double)curX, curHeight, (double)curZ);

            int temp_u = curX, temp_v = curZ;
            while (temp_u < 0) {
                temp_u += 3;
            }

            while (temp_v < 0) {
                temp_v += 3;
            }
            temp_v %= 3;
            temp_u %= 3;


            v->mTexCoord = glm::vec2((double)temp_u / 3.0, (double)temp_v / 3.0);
            v->mIndex = idx++;
            mVertices.push_back(v);

            positions.push_back(v->mPosition);
            texcoords.push_back(v->mTexCoord);
        }
    }


    // Generate the Faces and Edges
    vector<WE_Vertex*> vertices;
    for (int curX = 0; curX < mGridX - 1; ++curX) {
        for (int curZ = 0; curZ < mGridZ - 1; ++curZ) {
            vertices.clear();
            vertices.push_back(mVertices.at(curX*mGridX + curZ));
            vertices.push_back(mVertices.at(curX*mGridX + curZ+1));
            vertices.push_back(mVertices.at((curX+1)*mGridX + curZ+1));
            CreateFace(mEdges, mFaces, vertices);

            vertices.clear();
            vertices.push_back(mVertices.at((curX+1)*mGridX + curZ+1));
            vertices.push_back(mVertices.at((curX+1)*mGridX + curZ));
            vertices.push_back(mVertices.at(curX*mGridX + curZ));
            CreateFace(mEdges, mFaces, vertices);
        }
    }

    GetIndices(mFaces, indices);

    // Find the Vertex Normal using a weighted average using the vertex angles
    for (auto& vert : mVertices) {
        GetVertexNormal(vert, normals);
    }

    // OpenGL-ize all data
    mNumIndices = indices.size();

    InitOpenGLData(indices, positions, normals, texcoords);
}


GroundMesh::~GroundMesh() {

}

double GroundMesh::height(double currentHeight) {
    if (mWithHeight) {
        double height = 0;
        double var = mRand(mGen);
        if (var > 1.0) {
            height = 3.0;
        } else if (var < -1.0) {
            height = -3.0;
        }

        return height;

        // This following would give a smoother transission but would look too
        // close to the subdivided surface
        // if (currentHeight >= mMaxHeight || currentHeight <= mMinHeight) {
        //     return currentHeight;
        // }
        // // std::cout << "mRand(Gen): " << mRand(mGen) << endl;
        // return (currentHeight + mRand(mGen));
    } else {
        return 0;

    }
}

void GroundMesh::Render(std::shared_ptr<Program> shader) {
    assert(shader->IsInUse());

    glBindVertexArray(mVertexArray);
    glDrawElements(GL_TRIANGLES_ADJACENCY,
        mNumIndices,
        GL_UNSIGNED_INT,
        NULL);
    glBindVertexArray(0);
}

void GroundMesh::Subdivide(int numOfIteration) {

    if (numOfIteration < 1) {
        return;
    }

    vector<glm::vec3> positions;
    vector<glm::vec3> normals;
    vector<glm::vec2> texcoords;
    vector<unsigned int> indices;

    for (int i = 0; i < numOfIteration; ++i) {
        SubdivideMeshEntry(mEdges, mFaces, mVertices, indices);

        if (i != numOfIteration-1) {
            indices.clear();
            continue;
        }

        for (auto& vert : mVertices) {
            GetVertexNormal(vert, normals);
            positions.push_back(vert->mPosition);
            texcoords.push_back(vert->mTexCoord);
        }
        mNumIndices = indices.size();
    }

    InitOpenGLData(indices, positions, normals, texcoords);
}
