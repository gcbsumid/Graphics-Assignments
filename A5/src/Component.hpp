#ifndef COMPONENT_HPP
#define COMPONENT_HPP

#include <GL/glew.h>
#include <string>
#include <memory>

class Entity;

#include "Program.hpp"
#include "Mesh.hpp"
// TODO: AI Component
// TODO: Particle Component
// TODO: Physics Component

/******************************** Component ************************************/
class Component {
public:
    enum CompType {  
        COMPTYPE_DRAW = 0,
        COMPTYPE_CAMERA,
        COMPTYPE_LIGHT,
        COMPTYPE_AI,
        // COMPTYPE_PHYSICS,
        // COMPTYPE_PARTICLE,

        COMPTYPE_MAX
    } ;
        
    Component(std::shared_ptr<Entity>, CompType);
    ~Component();

    virtual void Update();

    CompType GetType() const;
    int GetID() const;

    virtual bool Render(Program*);
    std::weak_ptr<Entity> GetParent();

protected:
    std::weak_ptr<Entity> mParent;
    const CompType mType;
    const int mID;
};

/******************************** CameraComp ************************************/
class CameraComp : public Component {
public:
    CameraComp(std::shared_ptr<Entity> parent, float fov = 50.0f, float near = 0.5f, float far = 100.0f, float aspect = 4.0f/3.0f);
    virtual ~CameraComp();

    virtual void Update();
    virtual bool Render(Program*);

    // Determines how wide the view of the camera is
    float FieldOfView() const;
    void SetFieldOfView(const float fieldOfView);

    // The closest and farthest visible distance from the camera
    float NearPlane() const;
    float FarPlane() const;
    void SetNearAndFarPlanes(const float nearPlane, const float farPlane);

    // returns the unit vector representing the direction the camera
    // is facing, to the right of the camera, and top of the camera
    glm::vec3 Forward() const;
    glm::vec3 Right() const;
    glm::vec3 Up() const;

    // the combined camera transformation matrix
    glm::mat4 Matrix();

private:
    float mFieldOfView;
    float mNearPlane;
    float mFarPlane;
    float mViewportAspectRatio;
    glm::mat4 mCameraMatrix;
};

/******************************** DrawComp ************************************/
class DrawComp : public Component {
public:
    DrawComp(std::shared_ptr<Entity>);
    virtual ~DrawComp();

    virtual bool Render(Program*);

    void SetShader(Program* program);
    void SetMesh(std::shared_ptr<Mesh> mesh);
    std::string GetMaterialName() const;
    Program* GetShader() const;

private:
    std::weak_ptr<Mesh> mMesh;
    Program* mShader;
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
class LightComp : public Component {
public:
    LightComp(std::shared_ptr<Entity>, glm::vec3, glm::vec3, float, float);
    virtual ~LightComp();

    virtual void Update();
    virtual bool Render(Program*);

    void SetIntensities(glm::vec3);
    glm::vec3 GetIntensities() const;

    void SetColor(glm::vec3);
    glm::vec3 GetColor() const;

    void SetAttenuation(GLfloat);
    GLfloat GetAttenuation() const;

    void SetAmbientCoefficient(GLfloat);
    GLfloat GetAmbientCoefficient() const;

private:
    // Note: Specular and Diffuse Colour is in the material
    glm::vec3 mIntensities;
    glm::vec3 mColor;
    GLfloat mAttenuation;
    GLfloat mAmbientCoefficient;
};

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