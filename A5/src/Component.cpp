#include <memory>
#include <iostream>
#include <cassert>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Component.hpp"
#include "Entity.hpp"

using namespace std;

static int ID_COMP_COUNTER = 0;

/******************************** Component ************************************/

Component::Component(shared_ptr<Entity> parent, CompType type) 
    : mParent(parent)
    , mType(type)
    , mID(ID_COMP_COUNTER++)
{}

Component::~Component() {}

void Component::Update() {

}

bool Component::Render(Program*) {
    return false;
}

Component::CompType Component::GetType() const {
    return mType;
}

int Component::GetID() const {
    return mID;
}

weak_ptr<Entity> Component::GetParent() {
    return mParent;
}


/******************************** CameraComp ************************************/

CameraComp::CameraComp(shared_ptr<Entity> parent, float fov, float near, float far, float aspect) 
    : Component(parent, COMPTYPE_CAMERA) 
    , mFieldOfView(fov)
    , mNearPlane(near)
    , mFarPlane(far) 
    , mViewportAspectRatio(aspect)
{
    Update();
}

CameraComp::~CameraComp() {

}

void CameraComp::Update() {
    mCameraMatrix = glm::perspective(mFieldOfView, mViewportAspectRatio, mNearPlane, mFarPlane);
    auto parent = mParent.lock();
    mCameraMatrix = parent->GetRotate() * parent->GetTranslate() * mCameraMatrix;
}

bool CameraComp::Render(Program* shader) {
    GLint cameraMatrix = shader->Uniform("camera");
    GLint cameraPosition = shader->Uniform("cameraPos");

    if (cameraMatrix == -1 || cameraPosition == -1) {
        return false;
    }

    glUniformMatrix4fv(cameraMatrix, 1, GL_FALSE, glm::value_ptr(Matrix()));

    auto parent = mParent.lock();
    glm::vec4 pos = parent->GetTranslate() * glm::vec4(0,0,0,1);
    glUniform3f(cameraPosition, pos.x, pos.y, pos.z);
    return true;
}

float CameraComp::FieldOfView() const {
    return mFieldOfView;
}

void CameraComp::SetFieldOfView(const float fieldOfView) {
    mFieldOfView = fieldOfView;
}

float CameraComp::NearPlane() const {
    return mNearPlane;
}

float CameraComp::FarPlane() const {
    return mFarPlane;
}

void CameraComp::SetNearAndFarPlanes(const float nearPlane, const float farPlane) {
    assert(nearPlane > 0.0f);
    assert(farPlane > nearPlane);

    mNearPlane = nearPlane;
    mFarPlane = farPlane;
}

// void CameraComp::MoveCamera(float elapsedTime, glm::vec3 direction) {

// }

glm::vec3 CameraComp::Forward() const {
    auto parent = mParent.lock();
    glm::vec4 forward = glm::inverse(parent->GetRotate()) * glm::vec4(0,0,-1,1);
    forward = glm::vec4(forward[0], 0.0, forward[2], forward[3]);
    return glm::vec3(forward);
}

glm::vec3 CameraComp::Right() const {
    auto parent = mParent.lock();
    glm::vec4 right = glm::inverse(parent->GetRotate()) * glm::vec4(1,0,0,1);
    right = glm::vec4(right[0], 0.0, right[2], right[3]);
    return glm::vec3(right);
}

glm::vec3 CameraComp::Up() const {
    auto parent = mParent.lock();
    glm::vec4 up = glm::inverse(parent->GetRotate()) * glm::vec4(0,1,0,1);
    up = glm::vec4(0.0, up[1], 0.0, up[3]);
    return glm::vec3(up);
}

glm::mat4 CameraComp::Matrix() {
    return mCameraMatrix;
}


/******************************** DrawComp ************************************/
DrawComp::DrawComp(shared_ptr<Entity> parent) 
    : Component(parent, COMPTYPE_DRAW) {

}

DrawComp::~DrawComp() {

}

bool DrawComp::Render(Program* program) {
    assert(mMesh.use_count());

    auto mesh = mMesh.lock();
    auto parent = mParent.lock();

    program->SetUniform("modelTransform", parent->GetTransform());
    glBindBuffer(GL_ARRAY_BUFFER, mesh->mVertexBuffer);
    glBindVertexArray(mesh->mVertexArray);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->mIndexBuffer);
    glDrawElements(GL_TRIANGLES, mesh->mNumIndices, GL_UNSIGNED_INT, 0);

    return true;
}

void DrawComp::SetShader(Program* program) {
    assert(program);
    mShader = program;
}

void DrawComp::SetMesh(std::shared_ptr<Mesh> mesh) {
    assert(mesh.use_count());

    mMesh = mesh;
}

string DrawComp::GetMaterialName() const {
    assert(mMesh.use_count());
    auto mesh = mMesh.lock();
    return mesh->mMaterialName;
}

Program* DrawComp::GetShader() const {
    return mShader;
}

/******************************** AIComp ************************************/

AIComp::AIComp(shared_ptr<Entity> parent)
    : Component(parent, COMPTYPE_AI) {

}

AIComp::~AIComp() {
    // ClearAlgorithms()
}

void AIComp::Update() {
    // TODO: Updates
}

void AIComp::Action(double tick) {
    // TODO: Do some sort of action
}

// void AIComp::CreateAlogrithm(Alorithm::AlgoType type) {
//     // TODO: Create Algorithms
// }

// void AIComp::ClearAlogrithms() {

// }


/******************************** LightComp ************************************/

LightComp::LightComp(
    shared_ptr<Entity> parent,
    glm::vec3 color,
    glm::vec3 intensities,
    float attenuation,
    float ambientCoef
    ) 
    : Component(parent, COMPTYPE_LIGHT)
    , mIntensities(intensities)
    , mColor(color)
    , mAttenuation(attenuation)
    , mAmbientCoefficient(ambientCoef)
{

}

LightComp::~LightComp() {}

void LightComp::Update() {
    // Something?
}

bool LightComp::Render(Program* shader) {
    GLint position = shader->Uniform("light.position");
    GLint intensities = shader->Uniform("light.intensities");
    GLint attenuation = shader->Uniform("light.attenuation");
    GLint ambientCoefficient = shader->Uniform("light.ambientCoefficient");

    auto parent = mParent.lock();
    glm::vec4 pos = parent->GetTranslate() * glm::vec4(0,0,0,0);
    
    glUniform3f(position, pos.x, pos.y, pos.z);
    glUniform3f(intensities, mColor.x, mColor.y, mColor.z);
    glUniform1f(attenuation, mAttenuation);
    glUniform1f(ambientCoefficient, mAmbientCoefficient);
    return true;
}

void LightComp::SetIntensities(glm::vec3 intensities) {
    mIntensities = intensities;
}

glm::vec3 LightComp::GetIntensities() const {
    return mIntensities;
}

void LightComp::SetColor(glm::vec3 color) {
    mColor = color;
}

glm::vec3 LightComp::GetColor() const {
    return mColor;
}

void LightComp::SetAttenuation(GLfloat attenuation) {
    mAttenuation = attenuation;
}

GLfloat LightComp::GetAttenuation() const {
    return mAttenuation;
}

void LightComp::SetAmbientCoefficient(GLfloat ambientCoefficient) {
    mAmbientCoefficient = ambientCoefficient;
}

GLfloat LightComp::GetAmbientCoefficient() const {
    return mAmbientCoefficient;
}