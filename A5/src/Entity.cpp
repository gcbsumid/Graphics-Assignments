#include <cassert>
#include <memory>
#include <iostream>

#include "Entity.hpp"
#include <glm/gtc/matrix_transform.hpp>


using namespace std;

static int ID_ENT_COUNTER = 0;

Entity::Entity() {
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
    mTransform = mTranslate * mRotate * mScale;
    return mTransform;
}

void Entity::Scale(glm::vec3 scale) {
    glm::mat4 s = glm::scale(glm::mat4(), scale);
    mScale = mScale * s;
}

void Entity::Translate(glm::vec3 pos) {
    glm::mat4 t = glm::translate(glm::mat4(), pos);
    mTranslate = mTranslate * t;
}

void Entity::Rotate(char axis, float angle) {
    // angle = (angle/180 * M_PI);

    if (axis == 'x') {
        mRotate = glm::rotate(mRotate, angle, glm::vec3(1,0,0));
    } else if (axis == 'y') {
        mRotate = glm::rotate(mRotate, angle, glm::vec3(0,1,0));
    } else if (axis == 'z') {
        mRotate = glm::rotate(mRotate, angle, glm::vec3(0,0,1));
    } else {
        cerr << "Error: Invalid Axis " << axis << endl;
    }
}

void Entity::Rotate(glm::vec3 axis, float angle) {
    glm::mat4 r; 

    r = glm::rotate(r, angle, axis);

    mRotate = r * mRotate ;
}

void Entity::DisplayTransform() {
    cout << "Model Transform of Entity: " << mID << endl;
    cout << "\t  { " << mTransform[0][0] << " ,\t" << mTransform[0][1] << " ,\t" << mTransform[0][2] << " ,\t" <<mTransform[0][3] << "}" << endl;
    cout << "\t  { " << mTransform[1][0] << " ,\t" << mTransform[1][1] << " ,\t" << mTransform[1][2] << " ,\t" <<mTransform[1][3] << "}" << endl;
    cout << "\t  { " << mTransform[2][0] << " ,\t" << mTransform[2][1] << " ,\t" << mTransform[2][2] << " ,\t" <<mTransform[2][3] << "}" << endl;
    cout << "\t  { " << mTransform[3][0] << " ,\t" << mTransform[3][1] << " ,\t" << mTransform[3][2] << " ,\t" <<mTransform[3][3] << "}" << endl;
}

glm::mat4 Entity::GetScale() const {
    return mScale;
}
glm::mat4 Entity::GetTranslate() const {
    return mTranslate;
}
glm::mat4 Entity::GetRotate() const {
    return mRotate;
}

