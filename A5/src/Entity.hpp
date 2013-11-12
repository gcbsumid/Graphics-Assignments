#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <map>
#include <memory>

#include "Component.hpp"

class Entity {
public:
    Entity();
    virtual ~Entity();

    virtual void AddComponent(Component::CompType);

    std::shared_ptr<Component> GetComponent(Component::CompType) const;

    int GetID() const;
    glm::mat4 GetTransform() const;

    virtual void Scale(glm::vec3 scale);
    virtual void Translate(glm::vec3 pos);
    virtual void Rotate(char axis, double angle);

    // For Debug only
    void DisplayTransform();

private:
    std::map<Component::CompType, std::shared_ptr<Component>> mComponents;
    int mID;

    glm::mat4 mTransform;

    // TODO: Bounding Box
};

#endif