#version 330 core

const int MAX_POINT_LIGHTS = 2;
const int MAX_SPOT_LIGHTS = 1;

in vec3 fragVert;
in vec3 fragNormal;
in vec2 fragCoord;

out vec4 finalColour;

struct BaseLight {
    vec3 Color;             // Light color
    float AmbientIntensity; // Ambient intensity for phong
    float DiffuseIntensity; // Diffuse intensity for phong
};

// struct DirectionalLight {
//     BaseLight Base;
//     vec3 Direction;
// };

struct Attenuation {
    float Constant;
    float Linear;
    float Exp;
};

struct PointLight {
    BaseLight Base;
    vec3 Position;
    Attenuation Atten;
};

struct SpotLight {
    PointLight Base;
    vec3 Direction;
    float CutOff;
};

uniform int numPointLights;
uniform int numSpotLights;
// uniform DirectionalLight directionalLight;
uniform PointLight point_light[MAX_POINT_LIGHTS];
uniform SpotLight spot_light[MAX_SPOT_LIGHTS];
uniform vec3 camera_position;
uniform mat4 rotation_matrix;

// Material stuff
uniform float mat_specular_intensity;
uniform float shininess;

vec4 CalcLightInternal(BaseLight Light, vec3 LightDirection, vec3 Point, vec3 Normal) {
    vec4 ambient_color = vec4(Light.Color, 1.0) * Light.AmbientIntensity;
    // vec4 temp = vec4(0,0,1,0);

    // vec3 norm = (model_matrix * vec4(fragNormal,1.0f)).xyz;
    // vec3 temp = -Normal;
    float diffuse_factor = dot(Normal, -LightDirection); // <- take note of this

    vec4 diffuse_color = vec4(0,0,0,1);
    vec4 specular_color = vec4(0,0,0,1);

    if (diffuse_factor > 0) {
        diffuse_color = vec4(Light.Color, 1.0f) * Light.DiffuseIntensity * diffuse_factor;

        vec3 vertex_to_eye = normalize(camera_position - Point);
        vec3 light_reflect = normalize(reflect(LightDirection, Normal));
        float specular_factor = dot(vertex_to_eye, light_reflect);
        specular_factor = pow(specular_factor, shininess);
        if (specular_factor > 0) {
            specular_color = vec4(Light.Color, 1.0f) * mat_specular_intensity * specular_factor;
        }
        // diffuse_color = -rotation_matrix[2];
    } else {
        // diffuse_color = vec4(temp, 1.0);
    }

    return (ambient_color + diffuse_color + specular_color);
}

// vec4 CalcDirectionalLight(vec3 Normal) {
//     return CalcLightInternal(directionalLight.Base, directionalLight.Direction, Normal);
// }

vec4 CalcPointLight(PointLight l, vec3 Point, vec3 Normal) {
    vec3 light_direction = Point - l.Position;

    float Distance = length(light_direction);
    light_direction = normalize(light_direction);

    vec4 color = CalcLightInternal(l.Base, light_direction, Point, Normal);
    float attenuation = l.Atten.Constant + 
                        l.Atten.Linear * Distance +
                        l.Atten.Exp * Distance * Distance;

    return color / attenuation;
}

vec4 CalcSpotLight(SpotLight l, vec3 Point, vec3 Normal) {
    vec3 light_to_pixel = normalize(Point - l.Base.Position);
    float spot_factor = dot(light_to_pixel, l.Direction);

    if (spot_factor > l.CutOff) {   // In the spotlight
        vec4 Color = CalcPointLight(l.Base, Point, Normal);
        return Color * (1.0 - (1.0 - spot_factor)/(1.0 - l.CutOff));
    } else {
        return vec4(0,0,0,1); // Not in the spotlight
    }
}

float rand(vec2 n)
{
  return 0.5 + 0.5 * 
     fract(sin(dot(n.xy, vec2(12.9898, 78.233)))* 43758.5453);
}

void main() {
    // vec3 vert = (model_matrix * vec4(fragVert,1.0f)).xyz;
    vec3 norm = normalize((rotation_matrix * vec4(fragNormal,1.0f)).xyz);
    vec4 grass_color = vec4(vec3(0.0, 1.0, 0.0) * rand(fragVert.xy), 1.0); // the random noise value for grass
    vec4 total_light = vec4(0,0,0,1); // CalcDirectionalLight(fragNormal);                                         
                                                                                            
    for (int i = 0 ; i < numPointLights ; i++) {                                           
        total_light += CalcPointLight(point_light[i], fragVert, norm);                              
    }                                                                                       
                                                                                            
    for (int i = 0 ; i < numSpotLights ; i++) {                                            
        total_light += CalcSpotLight(spot_light[i], fragVert, norm);                                
    }                                                                                       
                                                                                            
    vec3 fragColor = (grass_color * total_light).xyz;   
    // vec3 fragColor = (total_light).xyz;   


    finalColour = vec4(fragColor.x, fragColor.y, fragColor.z, 1.0);
    // finalColour = vec4(1., 1.0);
}