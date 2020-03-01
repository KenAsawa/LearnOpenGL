#version 330 core
  
layout (location = 0) in vec3 position;
layout(location = 1) in vec3 normals;

uniform mat4 vm;
uniform mat4 pm;
uniform mat4 mm;

uniform vec3 light_color;
uniform vec3 light_position;
uniform vec3 object_color;
uniform vec3 view_position;
uniform bool colour;
uniform bool flag;
uniform bool lights;
uniform bool normalcol;
uniform bool greyscale;
uniform bool red;
uniform bool green;
uniform bool blue;

flat out vec3 fragment_position;
flat out vec3 normal;

smooth out vec3 fragment_positionS;
smooth out vec3 normalS;

flat out vec3 col;

void main()
{
	normal = mat3(transpose(inverse(mm))) * normals;
	normalS = mat3(transpose(inverse(mm))) * normals;
	fragment_position = vec3(mm * vec4(position, 1.0f));
	fragment_positionS = vec3(mm * vec4(position, 1.0f));
	gl_Position = pm * vm * mm * vec4(position, 1.0);
}