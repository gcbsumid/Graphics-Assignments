#ifndef GROUND_HPP
#define GROUND_HPP

#include <memory>
#include <string>

#include "Entity.hpp"
#include "Mesh.hpp"
#include "GroundTechnique.hpp"
#include "GroundMesh.hpp"
#include "Camera.hpp"

// TODO: We will do some noise texturing and bump mapping for the Ground
//      - Generate my vertices.
//      - Implement winged-edge data structure
//      - Create a simple shader
//      - Test
//      - Fractal mountain implementation
//      - Test
//      - Implement bump mapping shader

class Ground : public Entity {
public:
    Ground(const std::shared_ptr<Camera> camera,
           unsigned int gridX, 
           unsigned int gridZ,
           double maxHeight,
           double minHeight);

    virtual ~Ground();

    bool Render();

private:
    const std::weak_ptr<Camera> mCamera;
    std::shared_ptr<GroundTechnique> mGroundTech;
    std::shared_ptr<GroundMesh> mGroundMesh;
};

#endif