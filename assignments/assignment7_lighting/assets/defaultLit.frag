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

#define MAX_LIGHTS 2
uniform Light _Lights[MAX_LIGHTS];
uniform sampler2D _Texture;
uniform vec3 _CamPos;
uniform Material _Mat;

void main(){
	vec3 newNormals = normalize(fs_in.WorldNormals);
	vec3 lightColor = vec3(0.0f, 0.0f, 0.0f);
	vec3 camToFragVec = _CamPos - fs_in.WorldPos;
	for (int i = 0; i < MAX_LIGHTS; i++){
		vec3 lightToFragVec = _Lights[i].position - fs_in.WorldPos;
		vec3 halfVecPart = lightToFragVec + camToFragVec;
		lightColor += _Lights[i].color * _Mat.ambientK;
		lightColor += _Lights[i].color * _Mat.diffuseK * max(dot(newNormals, normalize(lightToFragVec)), 0);
		lightColor += _Lights[i].color * _Mat.specular * pow(max(dot(newNormals, normalize(halfVecPart / halfVecPart.length())), 0), _Mat.shininess);
	}
	vec4 newColor = texture(_Texture,fs_in.UV);
	FragColor = vec4(newColor.rgb * lightColor, 1.0);
}