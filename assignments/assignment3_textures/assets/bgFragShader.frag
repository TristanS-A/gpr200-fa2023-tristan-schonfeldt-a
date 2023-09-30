#version 450
out vec4 FragColor;
in vec2 UV;
uniform sampler2D _BGText;
uniform sampler2D _BG2Text;
uniform sampler2D _NoiseStuff;
uniform float _Time;
void main(){
	float noise = texture(_NoiseStuff, UV + _Time * 0.05).r;

	vec2 newUV = UV + noise * 0.1;

	vec4 colorA = texture(_BGText,newUV);
	vec4 colorB = texture(_BG2Text,newUV);
	vec3 color = mix(colorA.rgb,colorB.rgb,colorB.a * 0.5);


	FragColor = vec4(color, 1.0);
}