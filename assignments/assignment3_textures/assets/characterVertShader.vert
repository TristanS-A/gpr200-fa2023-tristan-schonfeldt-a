#version 450
layout(location = 0) in vec3 vPos;
layout(location = 1) in vec2 vUV;
out vec2 UV;
uniform vec2 _AspectRatio;
void main(){
	UV = vUV;
	gl_Position = vec4(vec2(vPos.x * (100 / _AspectRatio.x), vPos.y * (100 / _AspectRatio.y)), vPos.z,1.0);
}