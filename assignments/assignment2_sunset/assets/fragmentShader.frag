#version 450
out vec4 FragColor;
in vec2 UV;
uniform float _Time;
uniform vec2 _AspectRatio;
uniform vec3 _RightWaveBaseColor;
uniform vec3 _LeftWaveBaseColor;
uniform vec3 _SunColorDay;
uniform vec3 _SunColorNight;
uniform vec3 _SunsetColorTop;
uniform vec3 _SunsetColorBottom;
uniform vec3 _RegularSkyColor;
uniform vec3 _NightSkyTop;
uniform vec3 _NightSkyBottom;
uniform float _SunSpeed;
uniform float _SunRadius;
uniform float _LeftWaveSpeed;
uniform float _RightWaveSpeed;
void main(){
{
    //Normolizes UV cords
    vec2 UV = (2.0 * gl_FragCoord.xy - _AspectRatio) / _AspectRatio.y;

    //Calculates sin wave speed based on sun speed uniform
    float sunSinSpeed = sin(_Time * _SunSpeed);

    //Set the sky color of the sunset
    vec3 sunsetColor = mix(_SunsetColorBottom, _SunsetColorTop, UV.y);

    //Set the sky color of nighttime
    vec3 nighColor = mix(_NightSkyBottom, _NightSkyTop, UV.y);

    //Sets the sky color when the sun is rising
    vec3 risingColor = mix(sunsetColor, _RegularSkyColor, clamp(sunSinSpeed, 0.0, 1.0));

    //Sets the color when the sun is setting
    vec3 settingColor = mix(nighColor, sunsetColor, clamp(sunSinSpeed + 1.0, 0.0, 1.0));

    //Combines the sky color when rising and setting to both rise and set
    vec3 skyColor = mix(settingColor, risingColor, clamp(sunSinSpeed, 0, 1));

    //Creates the sun circle
    float sun = length(vec2(UV - vec2(0.0, sunSinSpeed - 0.6))) - _SunRadius;

    //Smooths the sun
    sun = smoothstep(0.0, 0.1, sun);

    //Sets the sun color based on setting or rising
    vec3 sunColor = mix(_SunColorNight, _SunColorDay, clamp(sunSinSpeed, 0.0, 1.0));

    sunColor = mix(sunColor, skyColor, sun);

    //Creates the waves moving left
    float wavesLeft = sin(2.0 * UV.x + _Time * _LeftWaveSpeed) * 0.1 - 0.5;

    //Smooths left waves
    wavesLeft = 1.0 - smoothstep(wavesLeft + 0.1, wavesLeft + 0.15, UV.y);

    //Adds left wave colors to all colors and also makes then darker when the sun is setting
    vec3 allColors = mix(sunColor, _LeftWaveBaseColor * clamp(sunSinSpeed + 1.0, 0.4, 1.0), wavesLeft);

    //Creates the waves moving left
    float wavesRight = sin(2.0 * UV.x - _Time * _RightWaveSpeed) * 0.2 - 0.5;

    //Smooths right waves
    wavesRight = 1.0 - smoothstep(wavesRight + 0.1, wavesRight + 0.15, UV.y);

    //Adds right wave colors to all colors and also makes then darker when the sun is setting
    allColors = mix(allColors, clamp(_RightWaveBaseColor * clamp(sunSinSpeed + 1.0, 0.4, 1.0), vec3(0.0), vec3(1.0)), wavesRight);

    // Output all colors to the screen
    FragColor = vec4(allColors,1.0);
}
}