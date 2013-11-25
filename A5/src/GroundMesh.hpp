#ifndef GROUNDMESH_HPP
#define GROUNDMESH_HPP

#include <glm/glm.hpp>

#include <string>
#include <vector>
#include <random> // C++11 random

#include "Mesh.hpp"

class GroundMesh : public Mesh {
public:
    GroundMesh(int gridX,
               int gridY,
               double maxHeight,
               double minHeight);

    virtual ~GroundMesh();

    void RunFractalAlgorithm(int numRecurse);

    virtual void Render(std::shared_ptr<Program> shader);

protected:

    void CreateFace(std::vector<unsigned int>& face_indices, 
                    std::vector<unsigned int>& indices);
    double height(double currentHeight);

    std::vector<WE_Vertex*> mVertices;
    std::vector<WE_Face*> mFaces;
    std::vector<WE_Edge*> mEdges;

    unsigned int mNumIndices;

    std::random_device mRD;
    std::mt19937 mGen;
    std::uniform_real_distribution<double> mRand;

    int mGridX;
    int mGridZ;
    double mMinHeight;
    double mMaxHeight;


};

#endif