#version 450
out vec4 FragColor;
in vec2 UV;
uniform vec3 _Color;
uniform float _Brightness;
uniform float _Time;
uniform vec2 _AspectRatio;
void main(){
{
    //Normolizes UV cords
    vec2 UV = (2.0 * gl_FragCoord.xy - _AspectRatio) / _AspectRatio.y;

    //Set the sky color of the sunset
    vec3 sunsetColor = mix(vec3(0.9,0.5,0.1),vec3(1.0,0.7, 1.0), UV.y);

    //Set the sky color of nighttime
    vec3 nighColor = mix(vec3(0.0, 0.0, 0.2), vec3(0.0, 0.0, 0.0), UV.y);

    //Sets the sky color when the sun is rising
    vec3 risingColor = mix(sunsetColor, vec3(0.0, 0.7, 1.0), clamp(abs(sin(_Time)), 0.0, 1.0));

    //Sets the color when the sun is setting
    vec3 settingColor = mix(nighColor, sunsetColor, clamp(sin(_Time) + 1.0, 0.0, 1.0));

    //Combines the sky color when rising and setting to both rise and set
    vec3 skyColor = mix(settingColor, risingColor, clamp(sin(_Time), 0, 1));

    //Creates the sun circle
    float sun = distance(vec2(UV.x, UV.y - sin(_Time) + 0.6), vec2(0.0));

    //Smooths the sun
    sun = smoothstep(0.5, 0.6, sun);

    //Sets the sun color based on setting or rising
    vec3 sunColor = mix(vec3(1.0, clamp(0.7 + clamp(0.0, 1.0, sin(_Time) * 0.3), 0.0, 1.0), clamp(0.4 + clamp(sin(_Time) * 0.3, 0.0, 1.0), 0.0, 1.0)), skyColor, sun);

    //Creates the waves moving left
    float wavesLeft = sin(2.0 * UV.x + _Time) * 0.1 - 0.5;

    //Smooths left waves
    wavesLeft = 1.0 - smoothstep(wavesLeft + 0.1, wavesLeft + 0.15, UV.y);

    //Adds left wave colors to all colors and also makes then darker when the sun is setting
    vec3 allColors = mix(sunColor, vec3(0.0, 0.0, 0.5 + clamp(sin(_Time), 0.0, 0.5)), wavesLeft);

    //Creates the waves moving left
    float wavesRight = sin(2.0 * UV.x - _Time * 1.5) * 0.2 - 0.5;

    //Smooths right waves
    wavesRight = 1.0 - smoothstep(wavesRight + 0.1, wavesRight + 0.15, UV.y);

    //Adds right wave colors to all colors and also makes then darker when the sun is setting
    allColors = mix(allColors, vec3(0.0, 0.0, 0.4 + clamp(sin(_Time), 0.0, 0.5)), wavesRight);

    // Output all colors to the screen
    FragColor = vec4(allColors,1.0);
}
}