#ifndef LIGHTING_TECHNIQUE_HPP
#define LIGHTING_TECHNIQUE_HPP

#include "Technique.hpp"
#include <glm/glm.hpp>
#include <vector>
#include <string>

struct BaseLight {
    glm::vec3 mColor;
    float mAmbientIntensity;
    float mDiffuseIntensity;

    BaseLight()
        : mColor(0.0f, 0.0f, 0.0f)
        , mAmbientIntensity(0.0f)
        , mDiffuseIntensity(0.0f) {
    }
};

// struct DirectionalLight : public BaseLight {

// };

struct PointLight : public BaseLight {
    glm::vec3 mPosition;
    struct {
        float mConstant;
        float mLinear;
        float mExp;
    } mAttenuation;

    PointLight() 
        : BaseLight()
        , mPosition(0.0f, 0.0f, 0.0f) {
            mAttenuation.mConstant = 1.0f;
            mAttenuation.mLinear = 0.0f;
            mAttenuation.mExp = 0.0f;
        }

};

struct SpotLight : public PointLight {
    glm::vec3 mDirection;
    float mCutOff;

    SpotLight() 
        : PointLight()
        , mDirection(0.0f, 0.0f, -1.0f)
        , mCutOff(0.0f) {}
};

class LightingTechnique : public Technique {
public:
    static const unsigned int MAX_POINT_LIGHTS = 1;
    static const unsigned int MAX_SPOT_LIGHTS = 1;

    LightingTechnique(const std::string& vertex_shader, const std::string& fragment_shader);

    void SetColorTextureUnit(unsigned int textureUnit);
    void SetPointLights(unsigned int numLights, const std::vector<std::shared_ptr<PointLight>>& lights);
    void SetSpotLights(unsigned int numLights, const std::vector<std::shared_ptr<SpotLight>>& lights);
    void SetCameraPosition(const glm::vec3& cameraPos);
    void SetShininess(float shininess);
    void SetMatSpecularIntensity(float specularIntensity);
    void SetPerspectiveMatrix(const glm::mat4& persp);
    void SetModelMatrix(const glm::mat4& model);
    void SetRotMatrix(const glm::mat4& rot);

private:
    GLuint mColorTextureUnitLocation;
    GLuint mCameraPositionLocation;
    GLuint mMatSpecularIntensityLocation;
    GLuint mMatShininessLocation;
    GLuint mNumPointLightsLocation;
    GLuint mNumSpotLightLocation;
    GLuint mPerspectiveLocation;
    GLuint mModelLocation;
    GLuint mRotMatrixLocation;

    struct {
        GLuint Color;
        GLuint AmbientIntensity;
        GLuint DiffuseIntensity;
        GLuint Position;
        struct {
            GLuint Constant;
            GLuint Linear;
            GLuint Exp;
        } Atten;
    } mPointLightsLocation[MAX_POINT_LIGHTS];

    struct {
        GLuint Color;
        GLuint AmbientIntensity;
        GLuint DiffuseIntensity;
        GLuint Position;
        GLuint Direction;
        GLuint CutOff;
        struct {
            GLuint Constant;
            GLuint Linear;
            GLuint Exp;
        } Atten;
    } mSpotLightsLocation[MAX_SPOT_LIGHTS];
};


#endif