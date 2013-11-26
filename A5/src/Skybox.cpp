#include <iostream>
#include "Skybox.hpp"
#include "Pipeline.hpp"

using namespace std;

Skybox::Skybox(const std::shared_ptr<Camera> camera) 
    : mCamera(camera) {
    // , mDrawType(GL_TRIANGLES)
    // , mDrawStart(0)
    // , mDrawCount(6*2*3) {
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

    // glGenBuffers(1, &mVBO);
    // glGenVertexArrays(1, &mVAO);
    // glBindVertexArray(mVAO);
    // glBindBuffer(GL_ARRAY_BUFFER, mVBO);

    // GLfloat vertexData[] = {
    //     //  X     Y     Z       U     V          Normal
    //     // bottom
    //     -1.0f,-1.0f,-1.0f,
    //      1.0f,-1.0f,-1.0f,
    //     -1.0f,-1.0f, 1.0f,
    //      1.0f,-1.0f,-1.0f,
    //      1.0f,-1.0f, 1.0f,
    //     -1.0f,-1.0f, 1.0f,

    //     // top
    //     -1.0f, 1.0f,-1.0f,
    //     -1.0f, 1.0f, 1.0f,
    //      1.0f, 1.0f,-1.0f,
    //      1.0f, 1.0f,-1.0f,
    //     -1.0f, 1.0f, 1.0f,
    //      1.0f, 1.0f, 1.0f, 

    //     // front
    //     -1.0f,-1.0f, 1.0f, 
    //      1.0f,-1.0f, 1.0f, 
    //     -1.0f, 1.0f, 1.0f, 
    //      1.0f,-1.0f, 1.0f, 
    //      1.0f, 1.0f, 1.0f, 
    //     -1.0f, 1.0f, 1.0f, 

    //     // back
    //     -1.0f,-1.0f,-1.0f, 
    //     -1.0f, 1.0f,-1.0f, 
    //      1.0f,-1.0f,-1.0f, 
    //      1.0f,-1.0f,-1.0f, 
    //     -1.0f, 1.0f,-1.0f, 
    //      1.0f, 1.0f,-1.0f, 

    //     // left
    //     -1.0f,-1.0f, 1.0f, 
    //     -1.0f, 1.0f,-1.0f, 
    //     -1.0f,-1.0f,-1.0f, 
    //     -1.0f,-1.0f, 1.0f, 
    //     -1.0f, 1.0f, 1.0f, 
    //     -1.0f, 1.0f,-1.0f, 

    //     // right
    //      1.0f,-1.0f, 1.0f, 
    //      1.0f,-1.0f,-1.0f, 
    //      1.0f, 1.0f,-1.0f, 
    //      1.0f,-1.0f, 1.0f, 
    //      1.0f, 1.0f,-1.0f, 
    //      1.0f, 1.0f, 1.0f
    // };


    // glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);
    // glEnableVertexAttribArray(mSkyboxTech->GetProgram()->Attrib("vert"));
    // glVertexAttribPointer(mSkyboxTech->GetProgram()->Attrib("vert"), 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), NULL);

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

    Pipeline pipeline;
    pipeline.SetPerspective(camera->GetPerspMatrix());
    pipeline.SetCamera(camera->GetPos(), camera->Forward(), camera->Up());

    const glm::mat4 transform = glm::transpose(GetTransform());

    // glm::mat4 temp =  pipeline.GetWVPTrans(transform);
    // cout << "GetWVPTrans Transform of Entity: " << mID << endl;
    // cout << "\t  { " << temp[0][0] << " ,\t" << temp[0][1] << " ,\t" << temp[0][2] << " ,\t" <<temp[0][3] << "}" << endl;
    // cout << "\t  { " << temp[1][0] << " ,\t" << temp[1][1] << " ,\t" << temp[1][2] << " ,\t" <<temp[1][3] << "}" << endl;
    // cout << "\t  { " << temp[2][0] << " ,\t" << temp[2][1] << " ,\t" << temp[2][2] << " ,\t" <<temp[2][3] << "}" << endl;
    // cout << "\t  { " << temp[3][0] << " ,\t" << temp[3][1] << " ,\t" << temp[3][2] << " ,\t" <<temp[3][3] << "}" << endl;

    
    mSkyboxTech->SetWVP(pipeline.GetWVPTrans(transform));

    // glm::mat4 view_matrix = camera->GetPerspMatrix();
    // view_matrix.transpose(view_matrix);
    // mSkyboxTech->SetWVP(view_matrix * transform);
    // DisplayTransform();

    // mSkyboxTech->SetModelMatrix(GetTransform());



    // glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    mCubemapTexture->Bind(mSkyboxTech->GetProgram());
    // glBindVertexArray(mVAO);
    // glDrawArrays(mDrawType, mDrawStart, mDrawCount);
    // glBindVertexArray(0);
    mSkyboxMesh->Render(mSkyboxTech->GetProgram());

    glCullFace(OldCullFaceMode);
    glDepthFunc(OldDepthFuncMode);
    return true;
}