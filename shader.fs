#version 330 core
out vec4 modelColor;

uniform vec3 color;

void main()
{
    modelColor = vec4(color, 1.0f);
}