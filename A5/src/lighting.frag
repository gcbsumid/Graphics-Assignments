#version 330 core

const int MAX_POINT_LIGHTS = 2;
const int MAX_SPOT_LIGHTS = 1;

in vec3 fragVert;
in vec3 fragNormal;
in vec2 fragCoord;

invariant out vec4 finalColour;

struct BaseLight {
    vec3 Color;             // Light color
    float AmbientIntensity; // Ambient intensity for phong
    float DiffuseIntensity; // Diffuse intensity for phong
};

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
// uniform DirectionalLight directional_light;
uniform PointLight point_light[MAX_POINT_LIGHTS];
uniform SpotLight spot_light[MAX_SPOT_LIGHTS];
uniform vec3 camera_position;
uniform mat4 rotation_matrix;
// uniform mat4 model_matrix;
// uniform mat4 rot_matrix;

// Material stuff
uniform float mat_specular_intensity;
uniform float shininess;
uniform sampler2D color_map;
uniform vec3 mat_color;

vec4 CalcLightInternal(BaseLight Light, vec3 LightDirection, vec3 Normal) {
    vec4 ambient_color = vec4(Light.Color, 1.0) * Light.AmbientIntensity;
    float diffuse_factor = dot(Normal, -LightDirection); // <- take note of this

    vec4 diffuse_color = vec4(0,0,0,1);
    vec4 specular_color = vec4(0,0,0,1);

    if (diffuse_factor > 0.0f) {
        diffuse_color = vec4(Light.Color, 1.0f) * Light.DiffuseIntensity * diffuse_factor;

        vec3 vertex_to_eye = normalize(camera_position - fragVert);
        vec3 light_reflect = normalize(reflect(LightDirection, Normal));
        float specular_factor = dot(vertex_to_eye, light_reflect);
        specular_factor = pow(specular_factor, shininess);
        if (specular_factor > 0) {
            specular_color = vec4(Light.Color, 1.0f) * mat_specular_intensity * specular_factor;
        }
        // diffuse_color = vec4(0,1,0,1);
    } else {
        // diffuse_color = vec4(-Normal,1);
    }

    return (ambient_color + diffuse_color + specular_color);
}

// vec4 CalcDirectionalLight(vec3 Normal) {
//     return CalcLightInternal(directional_light.Base, directional_light.Direction, Normal);
// }

vec4 CalcPointLight(PointLight l, vec3 Normal) {
    vec3 light_direction = normalize(fragVert - l.Position);

    float Distance = length(light_direction);
    light_direction = normalize(light_direction);

    vec4 color = CalcLightInternal(l.Base, light_direction, Normal);
    float attenuation = l.Atten.Constant + 
                        l.Atten.Linear * Distance +
                        l.Atten.Exp * Distance * Distance;

    return color / attenuation;
}

vec4 CalcSpotLight(SpotLight l, vec3 Normal) {
    vec3 light_to_pixel = normalize(fragVert - l.Base.Position);
    float spot_factor = dot(light_to_pixel, l.Direction);

    if (spot_factor > l.CutOff) {   // In the spotlight
        vec4 color = CalcLightInternal(l.Base.Base, l.Direction, Normal);
        float Distance = length(light_to_pixel);

        float attenuation = l.Base.Atten.Constant + 
                            l.Base.Atten.Linear * Distance +
                            l.Base.Atten.Exp * Distance * Distance;

        vec4 Color = color / attenuation;
        return Color * (1.0 - (1.0 - spot_factor)/(1.0 - l.CutOff));
    } else {
        return vec4(0,0,0,1); // Not in the spotlight
    }
}

void main() {
    // vec4 total_light = CalcDirectionalLight(fragNormal);       
    vec3 norm = normalize((rotation_matrix * vec4(fragNormal,1.0f)).xyz);
    vec4 total_light = vec4(0,0,0,1);    
                                                                                            
    for (int i = 0 ; i < numPointLights ; i++) {                                           
        total_light += CalcPointLight(point_light[i], norm);                              
    }                                                                                       
                                                                                            
    for (int i = 0 ; i < numSpotLights ; i++) {                                            
        total_light += CalcSpotLight(spot_light[i], norm);                                
    } 

    vec4 fragColor = vec4(0,0,0,1);
    if (mat_color == vec3(-1.0,-1.0,-1.0)){
        fragColor = texture(color_map, fragCoord.xy) * total_light;   
        // fragColor = vec4(0,0,0,1) * texture(color_map, fragCoord.xy) * total_light;   
    } else {
        fragColor = vec4(mat_color, 1.0) * total_light;   
        // fragColor = vec4(norm, 1.0);   
    }

    finalColour = vec4(fragColor.x, fragColor.y, fragColor.z, 1.0);
}