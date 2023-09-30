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
	mat2 rotMat = mat2(
                cos(_RotAngle), -sin(_RotAngle),
                sin(_RotAngle), cos(_RotAngle)
				);
	gl_Position = vec4(vec2(vPos.x * (_SpriteXY.x / _AspectRatio.x) + _XPos, vPos.y * (_SpriteXY.y / _AspectRatio.y) + _YPos) * rotMat, vPos.z,1.0);
}