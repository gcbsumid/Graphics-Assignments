#include <memory>
#include <iostream>
#include <cassert>
#include <GL/glew.h>

#include "Component.hpp"
#include "Entity.hpp"
#include "Camera.hpp"

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

// bool Component::Render(Program*) {
//     return false;
// }

Component::CompType Component::GetType() const {
    return mType;
}

int Component::GetID() const {
    return mID;
}

weak_ptr<Entity> Component::GetParent() {
    return mParent;
}

/******************************** DrawComp ************************************/
DrawComp::DrawComp(shared_ptr<Entity> parent) 
    : Component(parent, COMPTYPE_DRAW) {

}

DrawComp::~DrawComp() {
    delete mTechnique;
}

// TODO: Fix how you render objects. 
bool DrawComp::Render(std::shared_ptr<Camera> camera) {
    assert(mMesh.use_count());

    auto parent = mParent.lock();

    mTechnique->SetViewMatrix(camera->GetViewMatrix());
    mTechnique->SetModelMatrix(parent->GetTransform());

    mMesh->Render(mTechnique->GetProgram());

    // glBindBuffer(GL_ARRAY_BUFFER, mMesh->mVertexBuffer);
    // glBindVertexArray(mMesh->mVertexArray);

    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mMesh->mIndexBuffer);
    // glDrawElements(GL_TRIANGLES, mMesh->mNumIndices, GL_UNSIGNED_INT, 0);

    return true;
}

void DrawComp::SetTechnique(Technique* tech) {
    assert(tech);
    mTechnique = tech;
}

void DrawComp::SetMesh(std::shared_ptr<Mesh> mesh) {
    assert(mesh.use_count());

    mMesh = mesh;
}

Technique* DrawComp::GetTechnique() const {
    return mTechnique;
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

// LightComp::LightComp(
//     shared_ptr<Entity> parent,
//     glm::vec3 color,
//     glm::vec3 intensities,
//     float attenuation,
//     float ambientCoef
//     ) 
//     : Component(parent, COMPTYPE_LIGHT)
//     , mIntensities(intensities)
//     , mColor(color)
//     , mAttenuation(attenuation)
//     , mAmbientCoefficient(ambientCoef)
// {

// }

// LightComp::~LightComp() {}

// void LightComp::Update() {
//     // Something?
// }

// bool LightComp::Render(Program* shader) {
//     GLint position = shader->Uniform("light.position");
//     GLint intensities = shader->Uniform("light.intensities");
//     GLint attenuation = shader->Uniform("light.attenuation");
//     GLint ambientCoefficient = shader->Uniform("light.ambientCoefficient");

//     auto parent = mParent.lock();
//     glm::vec4 pos = parent->GetTranslate() * glm::vec4(0,0,0,0);
    
//     glUniform3f(position, pos.x, pos.y, pos.z);
//     glUniform3f(intensities, mColor.x, mColor.y, mColor.z);
//     glUniform1f(attenuation, mAttenuation);
//     glUniform1f(ambientCoefficient, mAmbientCoefficient);
//     return true;
// }

// void LightComp::SetIntensities(glm::vec3 intensities) {
//     mIntensities = intensities;
// }

// glm::vec3 LightComp::GetIntensities() const {
//     return mIntensities;
// }

// void LightComp::SetColor(glm::vec3 color) {
//     mColor = color;
// }

// glm::vec3 LightComp::GetColor() const {
//     return mColor;
// }

// void LightComp::SetAttenuation(GLfloat attenuation) {
//     mAttenuation = attenuation;
// }

// GLfloat LightComp::GetAttenuation() const {
//     return mAttenuation;
// }

// void LightComp::SetAmbientCoefficient(GLfloat ambientCoefficient) {
//     mAmbientCoefficient = ambientCoefficient;
// }

// GLfloat LightComp::GetAmbientCoefficient() const {
//     return mAmbientCoefficient;
// }