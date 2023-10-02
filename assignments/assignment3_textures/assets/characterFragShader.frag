#version 450
out vec4 FragColor;
in vec2 UV;
uniform sampler2D _CharacterText;
uniform float _CharAlpha;
void main(){
	//Sets texture color to UV cords
	vec4 color = texture(_CharacterText, UV);

	//Sets frag color with texture color and character alpha uniform
	FragColor = vec4(color.rgb, color.a * clamp(_CharAlpha, 0.0, 1.0));
}