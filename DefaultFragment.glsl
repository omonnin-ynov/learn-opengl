#version 330 core
struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_diffuse2;
    sampler2D texture_diffuse3;
    sampler2D texture_diffuse4;
    sampler2D texture_specular1;
    sampler2D texture_specular2;
    sampler2D texture_specular3;
    sampler2D texture_specular4;
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

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

uniform DirLight dirLight;
uniform vec3 viewPos;
uniform Material material;


vec3 calcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 calcPointLight(PointLight light, vec3 normal, vec3 FragPos, vec3 viewDir);

void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir =  normalize(viewPos - FragPos);

    vec3 result = calcDirLight(dirLight, norm, viewDir);

    for (int i = 0; i < NR_POINT_LIGHTS; i++) {
        result += calcPointLight(pointLights[i], norm, FragPos, viewDir);
    }

    FragColor = vec4(result, 1.0);
}

vec3 calcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    vec3 lightDir = normalize(-light.direction);
    vec3 reflectDir = reflect(-lightDir, normal);

    // TODO loop over all materials
    ambient = ambient + light.ambient * vec3(texture(material.texture_diffuse1, TexCoord));

    diffuse = diffuse + light.diffuse * max(dot(normal, lightDir), 0.0) * vec3(texture(material.texture_diffuse1, TexCoord));
    
    specular = specular + light.specular * pow(max(dot(viewDir, reflectDir), 0.0), material.shininess) * vec3(texture(material.texture_specular1, TexCoord));
    
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
    vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse1, TexCoord));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.texture_diffuse1, TexCoord));
    vec3 specular = light.specular * spec * vec3(texture(material.texture_specular1, TexCoord));

    return (ambient + diffuse + specular) * attenuation;
}
