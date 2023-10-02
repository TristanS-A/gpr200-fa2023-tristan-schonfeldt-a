#version 450
out vec4 FragColor;
in vec2 UV;
uniform sampler2D _BGText;
uniform sampler2D _BG2Text;
uniform sampler2D _NoiseStuff;
uniform float _TileSize;
uniform float _Time;
uniform vec4 _DistortionColor;
uniform float _DistortionSpeed;
uniform float _DistortionPower;
void main(){

	//Gets noise distortion multiplied by distortion power uniform
	float noise = texture(_NoiseStuff, UV + _DistortionSpeed * _Time * 0.05).r * _DistortionPower;

	//Calculates new UV from noise distortion
	vec2 newUV = _TileSize * UV + noise * 0.1 - _DistortionPower * 0.05;

	//Gets colors of textures from new UV cords plus distortion color uniform
	vec4 colorA = texture(_BGText,newUV) + noise * _DistortionColor;
	vec4 colorB = texture(_BG2Text,newUV) + noise * _DistortionColor;

	//Lerps between two texture colors
	vec3 color = mix(colorB.rgb,colorA.rgb,colorA.a * 0.5);

	FragColor = vec4(color, 1.0);
}