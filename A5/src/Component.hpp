#ifndef COMPONENT_HPP
#define COMPONENT_HPP

#include <GL/glew.h>
#include <string>

#include "Entity.hpp"

class Entity;

// #include "Program.h"
// TODO: Program and Shader
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
        
    Component(Entity*, CompType);
    ~Component();

    virtual void Update();

    // virtual bool Render(Program*);

private:
    const int mID;
    const CompType mType;
    std::weak_ptr<Entity> mParent;
};

/******************************** CameraComp ************************************/
class CameraComp : public Component {
public:
    CameraComp();
    virtual ~CameraComp();

    virtual void Update();
    // virtual bool Render(Program*);

    // Determines how wide the view of the camera is
    float FieldOfView() const;
    void SetFieldOfView(const float fieldOfView);

    // The closest and farthest visible distance from the camera
    float NearPlane() const;
    float FarPlane() const;
    void SetNearAndFarPlanes(const float nearPlane, const float farPlane);

    // move Camera function
    void MoveCamera(float elapsedTime, glm::vec3 direction);

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
};

/******************************** DrawComp ************************************/
class DrawComp : public Component {
public:
    DrawComp();
    virtual ~DrawComp();

    virtual void Update();
    virtual void Render(Program*);

    // void SetShader(Program* program);
    // void SetMesh(std::shared_ptr<Mesh> mesh);
    std::string GetMaterialName() const;
    // Program* GetShader() const;

private:
    // std::weak_ptr<Mesh> mMesh;
    // Program* mShader;
};

/******************************** AIComp ************************************/
class AIComp : public Component {
public:
    AIComp();
    virtual ~AIComp();

    virtual void Update();
    virtual void Render(Program*);

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
    LightComp();
    virtual ~LightComp();

    virtual void Update();
    virtual void Render(Program*);

    void SetPosition(glm::vec3);
    glm::vec3 GetPosition() const;

    void SetIntensity(glm::vec3);
    glm::vec3 GetIntensity() const;

    void SetAttenuation(GLfloat);
    GLfloat GetAttenuation() const;

    void SetAmbientCoefficient(GLfloat);
    GLfloat GetAmbientCoefficient() const;

private:
    glm::vec3 mColor;
    glm::vec4 mDiffuse;
    GLfloat mAttenuation;
    GLfloat mAmbient;
    glm::vec3 mPosition;


};

// /******************************** ParticleComp ************************************/
// class ParticleComp : public Component {
// public:
//     ParticleComp();
//     virtual ~ParticleComp();

//     virtual void Update();
//     virtual void Render(Program*);

// private:

// };

// /******************************** PhysicsComp ************************************/
// class PhysicsComp : public Component {
// public:
//     PhysicsComp();
//     virtual ~PhysicsComp();

//     virtual void Update();
//     virtual void Render(Program*);

// private:

// };

#endif