#version 330 core
out vec4 FragColor;

struct DirLight {
    vec3 direction;

    vec3 ambientLightColor;
    vec3 diffuseLightColor;
    vec3 specularLightColor;
};

struct PointLight {
    vec3 position;

    vec3 ambientLightColor;
    vec3 diffuseLightColor;
    vec3 specularLightColor;
};

in vec3 FragPos;
in vec3 Normal;

uniform vec3 viewPos;
uniform vec3 objectColor;
uniform int objectShine;
uniform DirLight dirLight;
uniform PointLight pointLight;

// function prototypes
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(-FragPos);
    vec3 result = (CalcDirLight(dirLight, norm, viewDir) +  CalcPointLight(pointLight, norm, FragPos, viewDir)) * objectColor;
    FragColor = vec4(result, 1.0);
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(Normal, halfwayDir), 0.0), objectShine);
    // combine results
    vec3 ambient = 0.1 * light.ambientLightColor;
    vec3 diffuse = diff * light.diffuseLightColor;
    vec3 specular = 0.5 * spec * light.specularLightColor;
    return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(Normal, halfwayDir), 0.0), objectShine);
    // combine results
    vec3 ambient = 0.1 * light.ambientLightColor;
    vec3 diffuse = diff * light.diffuseLightColor;
    vec3 specular = 0.5 * spec * light.specularLightColor;
    return (ambient + diffuse + specular);
}