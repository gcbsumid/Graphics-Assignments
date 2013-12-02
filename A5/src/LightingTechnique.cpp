#include <string>
#include <iostream>
#include <stdexcept>
#include <sstream>

#include <glm/glm.hpp>
#include "LightingTechnique.hpp"

using namespace std;

LightingTechnique::LightingTechnique(const std::string& vertex_shader, const std::string& fragment_shader) {
    if (!AddShader(vertex_shader, GL_VERTEX_SHADER)) {
        throw runtime_error("Can't load lighting vertex shader.");
    }

    if (!AddShader(fragment_shader, GL_FRAGMENT_SHADER)) {
        throw runtime_error("Can't load lighting fragment shader.");
    }

    if (!CompileShaders()) {
        throw runtime_error("Can't compile lighting shader program.");
    }

    if (vertex_shader != "./ground.vert") {
        // Screw it. I'll just leave it like this.
        mColorTextureUnitLocation = mProgram->Uniform("color_map");
    } 
    mRotMatrixLocation = mProgram->Uniform("rotation_matrix");
    mCameraPositionLocation = mProgram->Uniform("camera_position");
    mMatSpecularIntensityLocation = mProgram->Uniform("mat_specular_intensity");
    mMatShininessLocation = mProgram->Uniform("shininess");
    mNumPointLightsLocation = mProgram->Uniform("numPointLights");
    mNumSpotLightLocation = mProgram->Uniform("numSpotLights");
    mPerspectiveLocation = mProgram->Uniform("persp_matrix");
    mModelLocation = mProgram->Uniform("model_matrix");
    // mRotMatrixLocation = mProgram->Uniform("rot_matrix");

    stringstream ss;
    for (unsigned int i = 0 ; i < MAX_POINT_LIGHTS ; i++) {
        ss << "point_light[" << i << "].Base.Color";
        mPointLightsLocation[i].Color = mProgram->Uniform(ss.str().c_str());
        ss.str(std::string());

        ss << "point_light[" << i << "].Base.AmbientIntensity";
        mPointLightsLocation[i].AmbientIntensity = mProgram->Uniform(ss.str().c_str());
        ss.str(std::string());
        
        ss << "point_light[" << i << "].Position";
        mPointLightsLocation[i].Position = mProgram->Uniform(ss.str().c_str());
        ss.str(std::string());

        ss << "point_light[" << i << "].Base.DiffuseIntensity";
        mPointLightsLocation[i].DiffuseIntensity = mProgram->Uniform(ss.str().c_str());
        ss.str(std::string());

        ss << "point_light[" << i << "].Atten.Constant";
        mPointLightsLocation[i].Atten.Constant = mProgram->Uniform(ss.str().c_str());
        ss.str(std::string());

        ss << "point_light[" << i << "].Atten.Linear";
        mPointLightsLocation[i].Atten.Linear = mProgram->Uniform(ss.str().c_str());
        ss.str(std::string());

        ss << "point_light[" << i << "].Atten.Exp";
        mPointLightsLocation[i].Atten.Exp = mProgram->Uniform(ss.str().c_str());
        ss.str(std::string());
    }

    // TODO: Work on this part. 
    for (unsigned int i = 0 ; i < MAX_SPOT_LIGHTS ; i++) {
        ss << "spot_light[" << i << "].Base.Base.Color";
        mSpotLightsLocation[i].Color = mProgram->Uniform(ss.str().c_str());
        ss.str(std::string()); 

        ss << "spot_light[" << i << "].Base.Base.AmbientIntensity";
        mSpotLightsLocation[i].AmbientIntensity = mProgram->Uniform(ss.str().c_str());
        ss.str(std::string()); 

        ss << "spot_light[" << i << "].Base.Base.DiffuseIntensity";
        mSpotLightsLocation[i].DiffuseIntensity = mProgram->Uniform(ss.str().c_str());
        ss.str(std::string()); 

        ss << "spot_light[" << i << "].Base.Position";
        mSpotLightsLocation[i].Position = mProgram->Uniform(ss.str().c_str());
        ss.str(std::string()); 

        ss << "spot_light[" << i << "].Direction";
        mSpotLightsLocation[i].Direction = mProgram->Uniform(ss.str().c_str());
        ss.str(std::string()); 

        ss << "spot_light[" << i << "].CutOff";
        mSpotLightsLocation[i].CutOff = mProgram->Uniform(ss.str().c_str());
        ss.str(std::string()); 

        ss << "spot_light[" << i << "].Base.Atten.Constant";
        mSpotLightsLocation[i].Atten.Constant = mProgram->Uniform(ss.str().c_str());
        ss.str(std::string()); 

        ss << "spot_light[" << i << "].Base.Atten.Linear";
        mSpotLightsLocation[i].Atten.Linear = mProgram->Uniform(ss.str().c_str());
        ss.str(std::string()); 

        ss << "spot_light[" << i << "].Base.Atten.Exp";
        mSpotLightsLocation[i].Atten.Exp = mProgram->Uniform(ss.str().c_str());
        ss.str(std::string()); 
    }
}

void LightingTechnique::SetColorTextureUnit(unsigned int textureUnit) {
    glUniform1i(mColorTextureUnitLocation, textureUnit);
}

void LightingTechnique::SetPointLights(unsigned int numLights, const std::vector<std::shared_ptr<PointLight>>& lights) {
    glUniform1i(mNumPointLightsLocation, numLights);

    for (unsigned int i = 0; i < numLights; ++i) {
        glUniform3f(mPointLightsLocation[i].Color, lights.at(i)->mColor.x, lights.at(i)->mColor.y, lights.at(i)->mColor.z);
        glUniform1f(mPointLightsLocation[i].AmbientIntensity, lights.at(i)->mAmbientIntensity);
        glUniform1f(mPointLightsLocation[i].DiffuseIntensity, lights.at(i)->mDiffuseIntensity);
        glUniform3f(mPointLightsLocation[i].Position, lights.at(i)->mPosition.x, lights.at(i)->mPosition.y, lights.at(i)->mPosition.z);
        glUniform1f(mPointLightsLocation[i].Atten.Constant, lights.at(i)->mAttenuation.mConstant);
        glUniform1f(mPointLightsLocation[i].Atten.Linear, lights.at(i)->mAttenuation.mLinear);
        glUniform1f(mPointLightsLocation[i].Atten.Exp, lights.at(i)->mAttenuation.mExp);
    }
}

void LightingTechnique::SetSpotLights(unsigned int numLights, const std::vector<std::shared_ptr<SpotLight>>& lights) {
    glUniform1i(mNumSpotLightLocation, numLights);

    for (unsigned int i = 0 ; i < numLights ; i++) {
        glUniform3f(mSpotLightsLocation[i].Color, lights.at(i)->mColor.x, lights.at(i)->mColor.y, lights.at(i)->mColor.z);
        glUniform1f(mSpotLightsLocation[i].AmbientIntensity, lights[i]->mAmbientIntensity);
        glUniform1f(mSpotLightsLocation[i].DiffuseIntensity, lights[i]->mDiffuseIntensity);
        glUniform3f(mSpotLightsLocation[i].Position, lights.at(i)->mPosition.x, lights.at(i)->mPosition.y, lights.at(i)->mPosition.z);
        glm::vec3 dir = glm::normalize(lights[i]->mDirection);
        glUniform3f(mSpotLightsLocation[i].Direction, dir.x, dir.y, dir.z);
        glUniform1f(mSpotLightsLocation[i].CutOff, lights[i]->mCutOff);
        glUniform1f(mSpotLightsLocation[i].Atten.Constant, lights[i]->mAttenuation.mConstant);
        glUniform1f(mSpotLightsLocation[i].Atten.Linear,   lights[i]->mAttenuation.mLinear);
        glUniform1f(mSpotLightsLocation[i].Atten.Exp,      lights[i]->mAttenuation.mExp);
    }
}

void LightingTechnique::SetCameraPosition(const glm::vec3& cameraPos) {
    glUniform3f(mCameraPositionLocation, cameraPos.x, cameraPos.y, cameraPos.z);
}

void LightingTechnique::SetShininess(float shininess) {
    glUniform1f(mMatShininessLocation, shininess);
}

void LightingTechnique::SetMatSpecularIntensity(float specularIntensity) {
    glUniform1f(mMatSpecularIntensityLocation, specularIntensity);
}

void LightingTechnique::SetPerspectiveMatrix(const glm::mat4& persp) {
    glUniformMatrix4fv(mPerspectiveLocation, 1, GL_TRUE, glm::value_ptr(persp));
}

void LightingTechnique::SetModelMatrix(const glm::mat4& model) {
    glUniformMatrix4fv(mModelLocation, 1, GL_TRUE, glm::value_ptr(model));
}

void LightingTechnique::SetRotMatrix(const glm::mat4& rot) {
    glUniformMatrix4fv(mRotMatrixLocation, 1, GL_TRUE, glm::value_ptr(rot));
}