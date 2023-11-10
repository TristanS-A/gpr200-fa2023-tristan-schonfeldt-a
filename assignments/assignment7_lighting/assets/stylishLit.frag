#version 450
out vec4 FragColor;

in Surface{
	vec2 UV;
	vec3 WorldPos;
	vec3 WorldNormals;
}fs_in;

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
uniform sampler2D _Texture;
uniform vec3 _CamPos;
uniform Material _Mat;
uniform int _BlinnPhong;

float getLightEQ(vec3 newNormals, vec3 lightToFragVec, vec3 halfVecPart, vec3 camToFragVec){
	float lightVal = _Mat.ambientK;

	//My attempt at trying to make kindof basic cel-shading without any looking up of stuff because finding stuff is hard
	lightVal += _Mat.diffuseK * int(max(dot(newNormals, lightToFragVec), 0) > 0.0f);

	//Could not figure out how to do specular
	//lightVal += _Mat.diffuseK * int(pow(max(dot(newNormals, normalize(halfVecPart / halfVecPart.length())), 0), _Mat.shininess) > 0.1f) * _BlinnPhong;

	return lightVal;
}

void main(){
	vec3 newNormals = normalize(fs_in.WorldNormals);
	vec3 lightColor = vec3(0.0f, 0.0f, 0.0f);
	vec3 camToFragVec = normalize(_CamPos - fs_in.WorldPos);
	for (int i = 0; i < _CurrMaxLights; i++){
		vec3 lightToFragVec = normalize(_Lights[i].position - fs_in.WorldPos);
		vec3 halfVecPart = normalize(lightToFragVec + camToFragVec);
		lightColor += _Lights[i].color * getLightEQ(newNormals, lightToFragVec, halfVecPart, camToFragVec);
	}
	vec4 newColor = texture(_Texture,fs_in.UV);
	FragColor = vec4(newColor.rgb * lightColor, 1.0);
}