#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <GL/glew.h>
#include <string>
#include <iostream>

#include "Entity.hpp"
#include "Program.hpp"

class Camera : public Entity {
public:
    Camera(float fov = 60.0f,
               float near = 0.5f,
               float far = 100.0f,
               float aspect = 1.0f);

    // Determines how wide the view of the camera is

    // the combined camera transformation matrix
    glm::mat4 GetViewMatrix();
    virtual void UpdateViewMatrix();

    virtual glm::vec3 GetPos();

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

    bool Render(Program* shader);

    // void MoveCamera(float elapsedTime, glm::vec3 direction); 

private:
    float mFieldOfView;
    float mNearPlane;
    float mFarPlane;
    float mViewportAspectRatio;
    glm::mat4 mViewMatrix; // Perspective projection

    // glm::vec3 mPosition; 
    bool mNeedUpdate;
};

#endif