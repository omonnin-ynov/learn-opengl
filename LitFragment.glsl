#version 330 core
struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct DirLight {
    vec3 direction;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
#define NR_POINT_LIGHTS 4
uniform PointLight pointLights[NR_POINT_LIGHTS];

struct Light {
    //vec3 direction;
    vec3 position;
    vec3 direction;

    float cutOff;
    float outerCutOff;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

uniform Light light;
uniform vec3 viewPos;
uniform Material material;


vec3 calcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 calcPointLight(PointLight light, vec3 normal, vec3 FragPos, vec3 viewDir);

void main()
{
    // ambient
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoord));
    vec3 lightDir = normalize(light.position - FragPos);

    // spotlight
    float theta = dot(lightDir, normalize(-light.direction));
        
    // diffuse
    vec3 norm = normalize(Normal);

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * (vec3(texture(material.diffuse, TexCoord)));

    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoord));

    // attenuation
    float distance = length(light.position - FragPos);
    float attenuation = 1 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    // flashlight intensity
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    diffuse = diffuse * intensity;
    specular = specular * intensity;

    vec3 result = ambient + diffuse + specular;

    FragColor = vec4(result * attenuation, 1.0);

}  

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoord));

    vec3 diffuse = light.diffuse * max(dot(normal, lightDir), 0.0) * vec3(texture(material.diffuse, TexCoord));

    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 specular = light.specular * pow(max(dot(viewDir, reflectDir), 0.0), material.shininess) * vec3(texture(material.specular, TexCoord));

    return (ambient + diffuse + specular);
}


vec3 calcPointLight(PointLight light, vec3 normal, vec3 FragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - FragPos);
    // diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    // specular
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance);
    // combine results
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoord));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoord));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoord));

    return (ambient + diffuse + specular) * attenuation;
}
