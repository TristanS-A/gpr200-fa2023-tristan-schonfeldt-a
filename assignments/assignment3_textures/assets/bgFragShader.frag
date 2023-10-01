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
	float noise = texture(_NoiseStuff, UV + _DistortionSpeed * _Time * 0.05).r * _DistortionPower;

	vec2 newUV = _TileSize * (UV + noise * 0.1);

	vec4 colorA = texture(_BGText,newUV) + noise * _DistortionColor;
	vec4 colorB = texture(_BG2Text,newUV) + noise * _DistortionColor;
	vec3 color = mix(colorA.rgb,colorB.rgb,colorB.a * 0.5);


	FragColor = vec4(color, 1.0);
}