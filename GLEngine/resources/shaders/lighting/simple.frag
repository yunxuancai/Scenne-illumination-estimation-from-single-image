#version 400 core

out vec4 colorOutput;

uniform vec4 lightColor;


void main()
{
    colorOutput = vec4(lightColor);
}
