#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <glm/glm.hpp>
#include <map>
#include <memory>

#include "Component.hpp"
#include "Mesh.hpp"

class Entity {
public:
    Entity();
    virtual ~Entity();

    virtual void AddComponent(Component* comp);

    std::shared_ptr<Component> GetComponent(Component::CompType) const;

    int GetID() const;
    glm::mat4 GetTransform();

    virtual bool Render(std::shared_ptr<Program>& shader);
    virtual bool RenderMesh(std::shared_ptr<Program>& shader);

    virtual void Scale(glm::vec3 scale);
    virtual void Translate(glm::vec3 pos);
    virtual void Rotate(char axis, float angle);
    virtual void Rotate(glm::vec3 axis, float angle);

    void AttachColor(glm::vec3 color, unsigned int meshID);

    virtual glm::mat4 GetScale() const;
    virtual glm::mat4 GetTranslate() const;
    virtual glm::mat4 GetRotate() const;

    virtual bool AddMesh(const std::string& filename);

    // For Debug only
    void DisplayTransform();

protected:
    std::map<Component::CompType, std::shared_ptr<Component>> mComponents;
    int mID;

    glm::mat4 mTransform;

    std::shared_ptr<Mesh> mMesh;

    glm::vec3 mPosition;
    glm::vec3 mScale;
    glm::mat4 mRotate;
    // TODO: Bounding Box
};

#endif