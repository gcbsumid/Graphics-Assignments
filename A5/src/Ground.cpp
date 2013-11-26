#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <stdexcept>
#include "Ground.hpp"
#include "Pipeline.hpp"

using namespace std;

Ground::Ground(const std::shared_ptr<Camera> camera,
       unsigned int gridX, 
       unsigned int gridZ,
       double maxHeight,
       double minHeight) 
    : mCamera(camera)
    , mGroundTech(new GroundTechnique())
{
    if (!mGroundTech->Init()) {
        cerr << "Error at Ground Technique Initialization." << endl;
    }

    mGroundTech->Enable();
    // mGroundTech->SetTextureUnit(0); // I may not even need this.

    mGroundMesh = shared_ptr<GroundMesh>(new GroundMesh(gridX, gridZ, maxHeight, minHeight));
    // mGroundMesh->RunFractalAlgorithm(1);
}

Ground::~Ground() {
}

bool Ground::Render() {
    mGroundTech->Enable();

    glDisable(GL_CULL_FACE);

    auto camera = mCamera.lock();

    Pipeline pipeline;
    pipeline.SetPerspective(camera->GetPerspMatrix());
    pipeline.SetCamera(camera->GetPos(), camera->Forward(), camera->Up());
    mGroundTech->SetPerspectiveMatrix(pipeline.GetPerspectiveCameraTrans());

    // glm::mat4 cameraTrans = camera->GetGLMPerspMatrix();
    // cameraTrans *= glm::translate(glm::mat4(), -camera->GetPos());
    // cameraTrans *= camera->GetRotate();

    // mGroundTech->SetPerspectiveMatrix(cameraTrans);


    glm::mat4 temp =  pipeline.GetPerspectiveCameraTrans();
    cout << "PerspectiveCameraTrans of Entity: " << mID << endl;
    cout << "\t  { " << temp[0][0] << " ,\t" << temp[0][1] << " ,\t" << temp[0][2] << " ,\t" <<temp[0][3] << "}" << endl;
    cout << "\t  { " << temp[1][0] << " ,\t" << temp[1][1] << " ,\t" << temp[1][2] << " ,\t" <<temp[1][3] << "}" << endl;
    cout << "\t  { " << temp[2][0] << " ,\t" << temp[2][1] << " ,\t" << temp[2][2] << " ,\t" <<temp[2][3] << "}" << endl;
    cout << "\t  { " << temp[3][0] << " ,\t" << temp[3][1] << " ,\t" << temp[3][2] << " ,\t" <<temp[3][3] << "}" << endl;

    temp = glm::transpose(GetTransform());
    cout << "GetTransform of Entity: " << mID << endl;
    cout << "\t  { " << temp[0][0] << " ,\t" << temp[0][1] << " ,\t" << temp[0][2] << " ,\t" <<temp[0][3] << "}" << endl;
    cout << "\t  { " << temp[1][0] << " ,\t" << temp[1][1] << " ,\t" << temp[1][2] << " ,\t" <<temp[1][3] << "}" << endl;
    cout << "\t  { " << temp[2][0] << " ,\t" << temp[2][1] << " ,\t" << temp[2][2] << " ,\t" <<temp[2][3] << "}" << endl;
    cout << "\t  { " << temp[3][0] << " ,\t" << temp[3][1] << " ,\t" << temp[3][2] << " ,\t" <<temp[3][3] << "}" << endl;

    mGroundTech->SetModelMatrix(glm::transpose(GetTransform()));
    // mGroundTech->SetWVPMatrix(pipeline.GetWVPTrans(GetTransform()));

    // mGroundTech->SetWVP(pipeline.GetWVPTrans(transform));

    mGroundMesh->Render(mGroundTech->GetProgram());

    glEnable(GL_CULL_FACE);

    return true;
}