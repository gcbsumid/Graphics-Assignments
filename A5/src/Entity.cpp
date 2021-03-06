#include <cassert>
#include <memory>
#include <iostream>

#include "Entity.hpp"
#include <glm/gtc/matrix_transform.hpp>


using namespace std;

static int ID_ENT_COUNTER = 0;

Entity::Entity() 
    : mPosition(0,0,0)
    , mScale(1,1,1)
    , mHorizontalAngle(0.0f)
    , mVerticalAngle(0.0f) 
    , mHasAdjacency(false) {
    mID = ID_ENT_COUNTER++;
}

Entity::~Entity() {}

void Entity::AddComponent(Component* comp) {
    Component::CompType type = comp->GetType();
    if (mComponents.count(type)) {
        cout << "Replacing Component: " << type << endl;
    }

    mComponents[type] = shared_ptr<Component>(comp);
}

bool Entity::HasAdjacency() {
    return mHasAdjacency;
}

shared_ptr<Component> Entity::GetComponent(Component::CompType type) const {
    assert(type < Component::COMPTYPE_MAX);

    if (mComponents.count(type)) {
        return mComponents.at(type);
    }
    return nullptr;
}

int Entity::GetID() const {
    return mID;
}

glm::mat4 Entity::GetTransform() {
    mTransform = glm::scale(glm::mat4(), mScale);
    mTransform = mTransform * mRotate;
    mTransform = glm::translate(glm::mat4(), mPosition) * mTransform;
    return mTransform;
}

void Entity::Scale(glm::vec3 scale) {
    mScale = scale;
}

void Entity::Translate(glm::vec3 pos) {
    mPosition += pos;
}

void Entity::Rotate(char axis, float angle) {
    glm::mat4 r; 

    if (axis == 'x') {
        r = glm::rotate(r, angle, glm::vec3(1,0,0));
    } else if (axis == 'y') {
        r = glm::rotate(r, angle, glm::vec3(0,1,0));
        mHorizontalAngle += angle;
        if (mHorizontalAngle < 0.0f) {
           mHorizontalAngle += 360.0f;
        } else if (mHorizontalAngle >= 360.0f) {
            mHorizontalAngle -= 360.0f;
        }
    } else if (axis == 'z') {
        r = glm::rotate(r, angle, glm::vec3(0,0,1));
    } else {
        cerr << "Error: Invalid Axis " << axis << endl;
    }

    mRotate = r * mRotate;
}

void Entity::Rotate(glm::vec3 axis, float angle) {
    if (axis == glm::vec3(0,1,0)) {
        mHorizontalAngle += angle;
        if (mHorizontalAngle < 0.0f) {
           mHorizontalAngle += 360.0f;
        } else if (mHorizontalAngle >= 360.0f) {
            mHorizontalAngle -= 360.0f;
        }
    }

    glm::mat4 r = glm::rotate(glm::mat4(), angle, axis);

    mRotate = r * mRotate;
}

void Entity::SetPosition(glm::vec3 pos) {
    mPosition = pos;
}

glm::vec3 Entity::GetPos() {
    return mPosition;
}

float Entity::GetHorizontalRotationAngle() const {
    return mHorizontalAngle;
}

void Entity::DisplayTransform() {
    cout << "Model Transform of Entity: " << mID << endl;
    cout << "\t  { " << mTransform[0][0] << " ,\t" << mTransform[0][1] << " ,\t" << mTransform[0][2] << " ,\t" <<mTransform[0][3] << "}" << endl;
    cout << "\t  { " << mTransform[1][0] << " ,\t" << mTransform[1][1] << " ,\t" << mTransform[1][2] << " ,\t" <<mTransform[1][3] << "}" << endl;
    cout << "\t  { " << mTransform[2][0] << " ,\t" << mTransform[2][1] << " ,\t" << mTransform[2][2] << " ,\t" <<mTransform[2][3] << "}" << endl;
    cout << "\t  { " << mTransform[3][0] << " ,\t" << mTransform[3][1] << " ,\t" << mTransform[3][2] << " ,\t" <<mTransform[3][3] << "}" << endl;
}

glm::mat4 Entity::GetScale() const {
    return glm::scale(glm::mat4(), mScale);
}
glm::mat4 Entity::GetTranslate() const {
    return glm::translate(glm::mat4(), mPosition);
}
glm::mat4 Entity::GetRotate() const {
    return mRotate;
}

bool Entity::Render(std::shared_ptr<Program>& shader) {
    mMesh->Render(shader);
    return true;
}

bool Entity::RenderMesh(std::shared_ptr<Program>& shader) {
    mMesh->Render(shader);
    return true;
}

bool Entity::RenderEntry(std::shared_ptr<Program>& shader, unsigned int idx) {
    mMesh->RenderEntry(shader, idx);
    return true;
}


bool Entity::AddMesh(const string& filename, bool withAdjacency) {
    mMesh = shared_ptr<Mesh>(new Mesh(withAdjacency));
    mHasAdjacency = withAdjacency;

    return mMesh->LoadMesh(filename);
}

void Entity::AttachColor(glm::vec3 color, unsigned int meshID) {
    mMesh->AttachColor(color, meshID);
}

void Entity::Subdivide(int numOfIteration) {
    mMesh->Subdivide(numOfIteration);
}