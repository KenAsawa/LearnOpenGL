#version 330 core
  
layout (location = 0) in vec3 position;
layout(location = 1) in vec3 normals;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

uniform vec3 light_color;
uniform vec3 light_position;
uniform vec3 object_color;
uniform vec3 view_position;

smooth out vec3 smoothFragmentPosition;
smooth out vec3 smoothNormal;

flat out vec3 flatFragmentPosition;
flat out vec3 flatNormal;

void main()
{
	smoothFragmentPosition = vec3(modelMatrix * vec4(position, 1.0f));
	smoothNormal = mat3(transpose(inverse(modelMatrix))) * normals;
	flatFragmentPosition = vec3(modelMatrix * vec4(position, 1.0f));
	flatNormal = mat3(transpose(inverse(modelMatrix))) * normals;
	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(position, 1.0);
}