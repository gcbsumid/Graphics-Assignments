#ifndef SKYBOX_HPP
#define SKYBOX_HPP

#include <memory>
#include <string>

#include "Entity.hpp"
#include "SkyboxTechnique.hpp"
#include "Mesh.hpp"
#include "CubemapTexture.hpp"
#include "Camera.hpp"

class Skybox : public Entity {
public:
    Skybox(const std::shared_ptr<Camera> camera);
    virtual ~Skybox();

    bool Init(const std::string& PosXFilename,
              const std::string& NegXFilename,
              const std::string& PosYFilename,
              const std::string& NegYFilename,
              const std::string& PosZFilename,
              const std::string& NegZFilename);

    bool Render();

private:
    const std::weak_ptr<Camera> mCamera;
    std::shared_ptr<SkyboxTechnique> mSkyboxTech;
    std::shared_ptr<Mesh> mSkyboxMesh;
    std::shared_ptr<CubemapTexture> mCubemapTexture;
};

#endif