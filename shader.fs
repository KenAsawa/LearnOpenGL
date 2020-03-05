#version 330 core

out vec4 resultColor;

uniform vec3 light_color;
uniform vec3 light_position;
uniform vec3 object_color;
uniform vec3 view_position;
uniform int shininess;
uniform bool applySmoothing;

uniform bool posLightStatus;
uniform vec3 posAmbientLightColor;
uniform vec3 posDiffuseLightColor;
uniform vec3 posSpecularLightColor;

uniform bool dirLightStatus;
uniform vec3 dirLightDirection;
uniform vec3 dirLightAmbientColor;
uniform vec3 dirLightDiffuseColor;
uniform vec3 dirLightSpecularColor;

smooth in vec3 smoothFragmentPosition;
smooth in vec3 smoothNormal;

flat in vec3 flatFragmentPosition;
flat in vec3 flatNormal;

vec3 calculateDirLight(vec3, vec3, vec3);
vec3 calculatePosLight(vec3, vec3, vec3);

uniform sampler2D cyborgTexture;
in vec2 TexCoord;

void main()
{
	vec3 lightColor;
	vec3 posLight;
	vec3 dirLightColor;
	vec3 dirLight;

	if(posLightStatus){
		lightColor = light_color;
	}else{
		lightColor = vec3(0, 0, 0);
	}
	if(dirLightStatus){
		dirLightColor = light_color;
	}else{
		dirLightColor = vec3(0, 0, 0);
	}

	if(applySmoothing){
		posLight = calculatePosLight(lightColor, smoothFragmentPosition, smoothNormal);
		dirLight = calculateDirLight(dirLightColor, smoothFragmentPosition, smoothNormal);
	}
	else{
		posLight = calculatePosLight(lightColor, flatFragmentPosition, flatNormal);
		dirLight = calculateDirLight(dirLightColor, flatFragmentPosition, flatNormal);
	}

	//resultColor = vec4((posLight + dirLight) * object_color, 1.0f);
	vec2 tex_coord_invert = vec2(TexCoord.x, 1 - TexCoord.y);
	resultColor = texture(cyborgTexture, TexCoord);
} 

vec3 calculateDirLight(vec3 light_color, vec3 fragment_position, vec3 normal){
	//ambient
	float ambient_strength = 0.25f;
	vec3 ambient = ambient_strength * dirLightAmbientColor * light_color;

	//diffuse
	vec3 light_direction = normalize(-dirLightDirection);
	float diffuse_strength = 0.75f;  //use max so that it doesn't go negative
	vec3 diffuse = diffuse_strength * max(dot(normalize(normal), light_direction), 0.0) * dirLightDiffuseColor * light_color;

	//Specular
	vec3 view_direction = normalize(view_position - fragment_position);
	vec3 reflect_light_direction = reflect(-light_direction, normalize(normal));
	float specular_strength = 1.0f;
	vec3 specular = specular_strength * pow(max(dot(reflect_light_direction, view_direction), 0.0), shininess) * dirLightSpecularColor * light_color;
	
	return (ambient + diffuse + specular);
}


vec3 calculatePosLight(vec3 light_color, vec3 fragment_position, vec3 normal){
	//ambient
	float ambient_strength = 0.25f;
	vec3 ambient = ambient_strength * posAmbientLightColor * light_color;

	//diffuse
	vec3 light_direction = normalize(light_position - fragment_position);
	float diffuse_strength = 0.75f;  //use max so that it doesn't go negative
	vec3 diffuse = diffuse_strength * max(dot(normalize(normal), light_direction), 0.0) * posDiffuseLightColor * light_color;

	//Specular
	vec3 view_direction = normalize(view_position - fragment_position);
	vec3 reflect_light_direction = reflect(-light_direction, normalize(normal));
	float specular_strength = 1.0f;
	vec3 specular = specular_strength * pow(max(dot(reflect_light_direction, view_direction), 0.0), shininess) * posSpecularLightColor * light_color;
	
	return (ambient + diffuse + specular);
}