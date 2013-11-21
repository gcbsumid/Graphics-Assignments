#include <iostream>
#include "Skybox.hpp"

using namespace std;

Skybox::Skybox(const std::shared_ptr<Camera> camera) 
    : mCamera(camera) {
    // mCamera = camera;

}

Skybox::~Skybox() {
} 
 
bool Skybox::Init(const string& PosXFilename,
          const string& NegXFilename,
          const string& PosYFilename,
          const string& NegYFilename,
          const string& PosZFilename,
          const string& NegZFilename) {



    // Create the Skybox Technique
    mSkyboxTech = std::shared_ptr<SkyboxTechnique>(new SkyboxTechnique());
    if (!mSkyboxTech->Init()) {
        cerr << "Error at Skybox Technique initialization." << endl;
    }
    mSkyboxTech->Enable();
    mSkyboxTech->SetTextureUnit(0);


    mCubemapTexture = std::shared_ptr<CubemapTexture> (
        new CubemapTexture(PosXFilename,
           NegXFilename,
           PosYFilename,
           NegYFilename,
           PosZFilename,
           NegZFilename));

    if (!mCubemapTexture->Load()) {
        cerr << "Error: Unable to load texture." << endl;
        return false;
    }

    mSkyboxMesh = std::shared_ptr<Mesh>(new Mesh());
    return mSkyboxMesh->LoadMesh("resources/sphere.obj"); 
}

bool Skybox::Render() {
    mSkyboxTech->Enable();

    GLint OldCullFaceMode;
    glGetIntegerv(GL_CULL_FACE_MODE, &OldCullFaceMode);
    GLint OldDepthFuncMode;
    glGetIntegerv(GL_DEPTH_FUNC, &OldDepthFuncMode);    

    glCullFace(GL_FRONT);
    glDepthFunc(GL_LEQUAL);

    auto camera = mCamera.lock();

    glm::mat4 view_matrix = camera->GetViewMatrix();
    // view_matrix.transpose(view_matrix);
    mSkyboxTech->SetViewMatrix(view_matrix);
    // DisplayTransform();

    mSkyboxTech->SetModelMatrix(GetTransform());

    // glm::mat4 temp = camera->GetViewMatrix() * GetTransform();
    // cout << "temp Transform of Entity: " << mID << endl;
    // cout << "\t  { " << temp[0][0] << " ,\t" << temp[0][1] << " ,\t" << temp[0][2] << " ,\t" <<temp[0][3] << "}" << endl;
    // cout << "\t  { " << temp[1][0] << " ,\t" << temp[1][1] << " ,\t" << temp[1][2] << " ,\t" <<temp[1][3] << "}" << endl;
    // cout << "\t  { " << temp[2][0] << " ,\t" << temp[2][1] << " ,\t" << temp[2][2] << " ,\t" <<temp[2][3] << "}" << endl;
    // cout << "\t  { " << temp[3][0] << " ,\t" << temp[3][1] << " ,\t" << temp[3][2] << " ,\t" <<temp[3][3] << "}" << endl;

    mCubemapTexture->Bind(mSkyboxTech->GetProgram());
    mSkyboxMesh->Render(mSkyboxTech->GetProgram());

    glCullFace(OldCullFaceMode);
    glDepthFunc(OldDepthFuncMode);
    return true;
}