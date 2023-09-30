#version 450
out vec4 FragColor;
in vec2 UV;
uniform sampler2D _BGText;
uniform sampler2D _NoiseStuff;
uniform float _Time;
void main(){
	float noise = texture(_NoiseStuff, UV + _Time * 0.05).r;

	vec2 newUV = UV + noise * 0.1;

	FragColor = texture(_BGText, newUV);
}