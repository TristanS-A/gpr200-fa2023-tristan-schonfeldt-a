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

#define MAX_LIGHTS 1
uniform Light _Lights[1];
uniform sampler2D _Texture;

void main(){
	vec3 newNormals = normalize(fs_in.WorldNormals);
	vec3 lightColor = vec3(0.0f, 0.0f, 0.0f);
	for (int i = 0; i < MAX_LIGHTS; i++){
		lightColor += _Lights[i].color * max(dot(newNormals, normalize(_Lights[i].position - fs_in.WorldPos)), 0);
	}
	vec4 newColor = texture(_Texture,fs_in.UV);
	FragColor = vec4(newColor.rgb * lightColor, 1.0);
}