#ifndef GROUND_HPP
#define GROUND_HPP

#include <memory>
#include <string>

#include "Entity.hpp"
#include "Mesh.hpp"
#include "LightingTechnique.hpp"
#include "GroundMesh.hpp"
#include "Camera.hpp"

class Ground : public Entity {
public:
    Ground(const std::shared_ptr<Camera> camera,
           unsigned int gridX, 
           unsigned int gridZ,
           double maxHeight,
           double minHeight);

    virtual ~Ground();

    virtual bool Render();
    virtual bool RenderMesh(std::shared_ptr<Program>& shader);
    void AttachSpotLights(std::vector<std::shared_ptr<SpotLight>>& spotlight);
    void AttachPointLights(std::vector<std::shared_ptr<PointLight>>& pointLight);

    void Rebuild(bool withSubdivision, bool withFractal);

private:
    unsigned int mGridX;
    unsigned int mGridZ;
    double mMaxHeight;
    double mMinHeight;
    bool mHasSubdivided;

    const std::weak_ptr<Camera> mCamera;
    std::shared_ptr<LightingTechnique> mGroundTech;
    std::shared_ptr<GroundMesh> mGroundMesh;
    std::vector<std::shared_ptr<SpotLight>>  mSpotLights;
    std::vector<std::shared_ptr<PointLight>> mPointLights;
};

#endif