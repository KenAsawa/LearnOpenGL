#version 330 core

out vec4 result_color;

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
uniform bool blue;
uniform bool green;
uniform int shinyness;
uniform bool isSmooth;

uniform bool isPosLightOn;
uniform vec3 posAmbientLightColor;
uniform vec3 posDiffuseLightColor;
uniform vec3 posSpecularLightColor;

uniform bool isDirLightOn;
uniform vec3 dirLightDirection;
uniform vec3 dirLightAmbientColor;
uniform vec3 dirLightDiffuseColor;
uniform vec3 dirLightSpecularColor;

flat in vec3 fragment_position; //interpolated
flat in vec3 normal;

smooth in vec3 fragment_positionS; //interpolated
smooth in vec3 normalS;

vec3 calculateDirLight(vec3, vec3, vec3);
vec3 calculatePosLight(vec3, vec3, vec3);
vec3 calculateDirLight2(vec3, vec3, vec3);

void main()
{
	vec3 lightColor = light_color;
	vec3 posLight;
	if(!isPosLightOn){
		lightColor = vec3(0, 0, 0);
	}
	vec3 dirLightColor = light_color;
	vec3 dirLight;
	if(!isDirLightOn){
		dirLightColor = vec3(0, 0, 0);
	}
	if(isSmooth){
		posLight = calculatePosLight(lightColor, fragment_positionS, normalS);
		vec3 norm = normalize(normalS);
        vec3 viewDir = normalize(-fragment_positionS);
		dirLight = calculateDirLight(dirLightColor, norm, viewDir);
	}
	else{
		posLight = calculatePosLight(lightColor, fragment_position, normal);
		vec3 norm = normalize(normal);
        vec3 viewDir = normalize(-fragment_position);
		dirLight = calculateDirLight(dirLightColor, norm, viewDir);
	}

	result_color = vec4((posLight + dirLight) * object_color, 1.0f);
} 

vec3 calculateDirLight(vec3 light_color, vec3 normal, vec3 viewDir){
	vec3 lightDir = normalize(-dirLightDirection);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shinyness);
    // combine results
    vec3 ambient  = dirLightAmbientColor * 0.1 * light_color;
    vec3 diffuse  = dirLightDiffuseColor * diff *  light_color;
    vec3 specular = dirLightSpecularColor * spec * 0.5 * light_color;
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
	vec3 specular = specular_strength * pow(max(dot(reflect_light_direction, view_direction), 0.0), shinyness) * posSpecularLightColor * light_color;
	
	vec3 color = (specular + diffuse + ambient);
	return color;
}