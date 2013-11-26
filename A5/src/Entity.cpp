#include <cassert>
#include <memory>
#include <iostream>

#include "Entity.hpp"
#include <glm/gtc/matrix_transform.hpp>


using namespace std;

static int ID_ENT_COUNTER = 0;

Entity::Entity() 
    : mPosition(0,0,0)
    , mScale(1,1,1) {
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
    mTransform *= mRotate;
    mTransform *= glm::translate(glm::mat4(), mPosition);
    // mTransform = 
    // mScale * mRotate * 
    return mTransform;
}

void Entity::Scale(glm::vec3 scale) {
    mScale = scale;
}

void Entity::Translate(glm::vec3 pos) {
    // cout << "Moving towards: " << pos.x << ", " << pos.y << ", " << pos.z << endl;
    mPosition += pos;
    // cout << "Position is now at: " << mPosition.x << ", " << mPosition.y << ", " << mPosition.z << endl;
}

void Entity::Rotate(char axis, float angle) {
    // angle = (angle/180 * M_PI);
    glm::mat4 r; 

    if (axis == 'x') {
        r = glm::rotate(r, angle, glm::vec3(1,0,0));
    } else if (axis == 'y') {
        r = glm::rotate(r, angle, glm::vec3(0,1,0));
    } else if (axis == 'z') {
        r = glm::rotate(r, angle, glm::vec3(0,0,1));
    } else {
        cerr << "Error: Invalid Axis " << axis << endl;
    }

    mRotate = r * mRotate;
}

void Entity::Rotate(glm::vec3 axis, float angle) {
    glm::mat4 r; 

    r = glm::rotate(r, angle, axis);

    mRotate = r * mRotate;
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

