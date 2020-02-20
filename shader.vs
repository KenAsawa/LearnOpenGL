#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

flat out vec3 fFragPos;
flat out vec3 fNormal;
smooth out vec3 sFragPos;
smooth out vec3 sNormal;
out vec3 LightPos;

uniform vec3 lightPos; // we now define the uniform in the vertex shader and pass the 'view space' lightpos to the fragment shader. lightPos is currently in world space.
uniform bool applySmoothing;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    if(applySmoothing){
        gl_Position = projection * view * model * vec4(aPos, 1.0);
        sFragPos = vec3(view * model * vec4(aPos, 1.0));
        sNormal = mat3(transpose(inverse(view * model))) * aNormal;
        LightPos = vec3(view * vec4(lightPos, 1.0)); // Transform world-space light position to view-space light position
	}else{
        gl_Position = projection * view * model * vec4(aPos, 1.0);
        fFragPos = vec3(view * model * vec4(aPos, 1.0));
        fNormal = mat3(transpose(inverse(view * model))) * aNormal;
        LightPos = vec3(view * vec4(lightPos, 1.0)); // Transform world-space light position to view-space light position
	}

}
