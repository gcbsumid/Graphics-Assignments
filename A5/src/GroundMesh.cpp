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

            v->mPosition = glm::vec3((double)curX, curHeight, (double)curZ);
            v->mTexCoord = glm::vec2(0,0);
            v->mIndex = idx++;
            mVertices.push_back(v);

            // cout << v->mIndex << ": " << v->mPosition.x << ", " << v->mPosition.y << ", " << v->mPosition.z << endl;

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
            CreateFace(mEdges, mFaces, vertices, indices);

            vertices.clear();
            vertices.push_back(mVertices.at((curX+1)*mGridX + curZ+1));
            vertices.push_back(mVertices.at((curX+1)*mGridX + curZ));
            vertices.push_back(mVertices.at(curX*mGridX + curZ));
            CreateFace(mEdges, mFaces, vertices, indices);
        }
    }
    // cout << "I'm almost done creating Ground!" << endl;

    // Find the Vertex Normal using a weighted average using the vertex angles
    for (auto& vert : mVertices) {
        GetVertexNormal(vert, normals);
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
    //     height = 3.0;
    // } else if (var < -1.0) {
    //     height = -3.0;
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

        cout << "face size: " << mFaces.size() << endl;
        cout << "edge size: " << mEdges.size() << endl;
        cout << "mNumIndices size " << indices.size() << endl;

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
        cout << "mNumIndices size " << mNumIndices << endl;
    }

    InitOpenGLData(indices, positions, normals, texcoords);
}
