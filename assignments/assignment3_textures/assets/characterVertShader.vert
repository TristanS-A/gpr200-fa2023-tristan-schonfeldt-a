#version 450
layout(location = 0) in vec3 vPos;
layout(location = 1) in vec2 vUV;
out vec2 UV;
uniform vec2 _AspectRatio;
uniform vec2 _SpriteXY;
uniform float _XPos;
uniform float _YPos;
uniform float _RotAngle;
void main(){
	UV = vUV;

	//Sets up rotation matrix
	mat2 rotMat = mat2(
                cos(_RotAngle), -sin(_RotAngle),
                sin(_RotAngle), cos(_RotAngle)
				);

	//Sets up character pos
	vec2 CharPos = vec2(_XPos, _YPos);

	//Rotates character aroung the origin
	vec2 scaledChar = vec2(vPos.x * (_SpriteXY.x / _AspectRatio.x), vPos.y * (_SpriteXY.y / _AspectRatio.y));

	//Adds player pos
	gl_Position = vec4(rotMat * scaledChar + CharPos, vPos.z,1.0);
}