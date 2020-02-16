#version 330 core
out vec4 modelColor;

uniform vec3 objectColor;
uniform vec3 lightColor;

void main()
{
    modelColor = vec4(lightColor * objectColor, 1.0f);
}