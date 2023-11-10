#version 450
layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 vUV;

uniform mat4 _Model;
uniform vec3 _NWorldVec;
uniform mat4 _ViewProjection;

out vec2 UV;
out vec3 finalLightColor;

struct Light {
    vec3 position;
    vec3 color;
};

struct Material {
    float ambientK;
    float diffuseK;
    float shininess;
    float specular;
};

#define MAX_LIGHTS 5
uniform int _CurrMaxLights;
uniform Light _Lights[MAX_LIGHTS];
uniform vec3 _CamPos;
uniform Material _Mat;
uniform int _BlinnPhong;

float getLightEQ(vec3 newNormals, vec3 lightToFragVec, vec3 halfVecPart, vec3 camToFragVec){
    float lightVal = _Mat.ambientK;
    lightVal += _Mat.diffuseK * max(dot(newNormals, lightToFragVec), 0);

    //My attempt at branchless programming but idk if the cost of the extra math is worth it
    //lightVal += _Mat.specular * pow(max(dot(newNormals, normalize(halfVecPart / halfVecPart.length())) * _BlinnPhong + dot(reflect(-lightToFragVec, newNormals), camToFragVec) * abs(_BlinnPhong - 1), 0), _Mat.shininess);

    //The branch in case its less cost than the extra math
    if (_BlinnPhong == 1){
        lightVal += _Mat.specular * pow(max(dot(newNormals, normalize(halfVecPart / halfVecPart.length())), 0), _Mat.shininess);
    }
    else {
        lightVal += _Mat.specular * pow(max(dot(reflect(-lightToFragVec, newNormals), camToFragVec), 0), _Mat.shininess);
    }
    return lightVal;
}

void main(){
    UV = vUV;
    vec3 WorldPos = vec3(_Model * vec4(vPos,1.0));
    vec3 WorldNormals = transpose(inverse(mat3(_Model))) * vNormal;

    vec3 newNormals = normalize(WorldNormals);
    vec3 lightColor = vec3(0.0f, 0.0f, 0.0f);
    vec3 camToFragVec = normalize(_CamPos - WorldPos);
    for (int i = 0; i < _CurrMaxLights; i++){
        vec3 lightToFragVec = normalize(_Lights[i].position - WorldPos);
        vec3 halfVecPart = normalize(lightToFragVec + camToFragVec);
        lightColor += _Lights[i].color * getLightEQ(newNormals, lightToFragVec, halfVecPart, camToFragVec);
    }

    finalLightColor = lightColor;
    gl_Position = _ViewProjection * _Model * vec4(vPos,1.0);
}