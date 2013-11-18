#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <glm/glm.hpp>
#include <map>
#include <memory>

#include "Component.hpp"

class Entity {
public:
    Entity();
    virtual ~Entity();

    virtual void AddComponent(Component* comp);

    std::shared_ptr<Component> GetComponent(Component::CompType) const;

    int GetID() const;
    glm::mat4 GetTransform() const;

    virtual void Scale(glm::vec3 scale);
    virtual void Translate(glm::vec3 pos);
    virtual void Rotate(char axis, float angle);
    virtual void Rotate(glm::vec3 axis, float angle);


    virtual glm::mat4 GetScale() const;
    virtual glm::mat4 GetTranslate() const;
    virtual glm::mat4 GetRotate() const;

    // For Debug only
    void DisplayTransform();

private:
    std::map<Component::CompType, std::shared_ptr<Component>> mComponents;
    int mID;

    glm::mat4 mTransform;

    glm::mat4 mTranslate;
    glm::mat4 mRotate;
    glm::mat4 mScale;
    // TODO: Bounding Box
};

#endif