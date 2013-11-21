#ifndef COMPONENT_HPP
#define COMPONENT_HPP

#include <GL/glew.h>
#include <string>
#include <memory>
#include "Program.hpp"
#include "Mesh.hpp"
#include "Technique.hpp"

class Entity;
class Camera;

// TODO: AI Component
// TODO: Particle Component
// TODO: Physics Component

/******************************** Component ************************************/
class Component {
public:
    enum CompType {  
        COMPTYPE_DRAW = 0,
        COMPTYPE_LIGHT,
        COMPTYPE_AI,
        // COMPTYPE_PHYSICS,
        // COMPTYPE_PARTICLE,

        COMPTYPE_MAX
    } ;
        
    Component(std::shared_ptr<Entity>, CompType);
    virtual ~Component();

    virtual void Update();

    CompType GetType() const;
    int GetID() const;

    // virtual bool Render(Program*);
    std::weak_ptr<Entity> GetParent();

protected:
    std::weak_ptr<Entity> mParent;
    const CompType mType;
    const int mID;
};

/******************************** DrawComp ************************************/
class DrawComp : public Component {
public:
    DrawComp(std::shared_ptr<Entity>);
    virtual ~DrawComp();

    virtual bool Render(std::shared_ptr<Camera> camera);

    void SetTechnique(Technique* tech);
    void SetMesh(std::shared_ptr<Mesh> mesh);
    void SetTexture(std::shared_ptr<Texture> texture);

    Technique* GetTechnique() const;

protected:
    std::shared_ptr<Mesh> mMesh;
    Technique* mTechnique;
};

/******************************** AIComp ************************************/
class AIComp : public Component {
public:
    AIComp(std::shared_ptr<Entity>);
    virtual ~AIComp();

    virtual void Update();

    void Action(double);
    // void GenerateAlgorithm(ALGORITHM);
    // void ClearAlgorithms();

private:
    // TODO: AI Algorithms
    // std::vector<std::unique_ptr<Algorithm>> mAlgorithms; 
};

/******************************** LightComp ************************************/

// TODO: move this all into a lighting technique
// class LightComp : public Component {
// public:
//     LightComp(std::shared_ptr<Entity>, glm::vec3, glm::vec3, float, float);
//     virtual ~LightComp();

//     virtual void Update();
//     virtual bool Render(Program*);

//     void SetIntensities(glm::vec3);
//     glm::vec3 GetIntensities() const;

//     void SetColor(glm::vec3);
//     glm::vec3 GetColor() const;

//     void SetAttenuation(GLfloat);
//     GLfloat GetAttenuation() const;

//     void SetAmbientCoefficient(GLfloat);
//     GLfloat GetAmbientCoefficient() const;

// private:
//     // Note: Specular and Diffuse Colour is in the material
//     glm::vec3 mIntensities;
//     glm::vec3 mColor;
//     GLfloat mAttenuation;
//     GLfloat mAmbientCoefficient;
// };

// /******************************** ParticleComp ************************************/
// class ParticleComp : public Component {
// public:
//     ParticleComp(shared_ptr<Entity>);
//     virtual ~ParticleComp();

//     virtual void Update();
//     virtual void Render(Program*);

// private:

// };

// /******************************** PhysicsComp ************************************/
// class PhysicsComp : public Component {
// public:
//     PhysicsComp(shared_ptr<Entity>);
//     virtual ~PhysicsComp();

//     virtual void Update();
//     virtual void Render(Program*);

// private:

// };

#endif