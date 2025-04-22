#version 330 core

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
}; 
  
out vec4 FragColor;

in vec3 Normal;  
in vec3 FragPos;  
in vec3 Color;
in vec4 fragPosLight;

uniform Material material;
uniform vec3 viewPos;
uniform sampler2D shadowMap;

struct PointLight {    
    vec3 position;
    vec3 color;
    
    float constant;
    float linear;
    float quadratic;  

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};  

struct SpotLight {    
    vec3 position;
    vec3 direction;
    vec3 color;
    
    float constant;
    float linear;
    float quadratic;  

    float cutOff;
    float outerCutOff;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};  

struct DirectionalLight {
    vec3 direction;
    vec3 color;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform DirectionalLight directionalLight;
#define NR_POINT_LIGHTS 16
#define NR_SPOT_LIGHTS 16
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLights[NR_SPOT_LIGHTS];


vec3 CalculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir);
vec3 CalculatePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);  
vec3 CalculateSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

const bool blinn = true;
const float gamma = 2.2f;
const float ambientMultiplier = 10.0f;

void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 result = CalculateDirectionalLight(directionalLight, norm, viewDir);

    for(int i = 0; i < NR_POINT_LIGHTS; i++){
        if(pointLights[i].constant == 0)
            continue;
        result += CalculatePointLight(pointLights[i], norm, FragPos, viewDir);
    }

    for(int i = 0; i < NR_SPOT_LIGHTS; i++){
        if(spotLights[i].constant == 0)
            continue;
        result += CalculateSpotLight(spotLights[i], norm, FragPos, viewDir);
    }
    
    FragColor = vec4(result, 1.0f);
    FragColor.rgb = pow(FragColor.rgb, vec3(gamma));
    vec3 lightCoords = fragPosLight.xyz / fragPosLight.w;
    FragColor = texture(shadowMap, lightCoords.xy);
}

vec3 CalculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir)
{
    vec3 ambientColor = pow(material.ambient * ambientMultiplier, vec3(1 / gamma));
    vec3 diffuseColor = pow(material.diffuse, vec3(1 / gamma));
    vec3 specularColor = pow(material.specular, vec3(1 / gamma));

    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading

    float spec;
    if(blinn)
    {
        vec3 halfwayDir = normalize(-directionalLight.direction + viewDir);  
        spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess * 128);
    }
    else
    {
        vec3 reflectDir = reflect(directionalLight.direction, normal);
        spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess * 128);
    }

    float shadow = 0.0f;
    vec3 lightCoords = fragPosLight.xyz / fragPosLight.w;
    if (lightCoords.z <= 1.0f)
    {
        lightCoords = (lightCoords + 1.0f) / 2.0f;

        float closestDepth = texture(shadowMap, lightCoords.xy).r;
        float currentDepth = lightCoords.z;

        if (currentDepth > closestDepth)
        {
            shadow = 1.0f;
        }
    }

    // combine results
    vec3 ambient  = light.color * light.ambient * ambientColor;
    vec3 diffuse  = light.color * light.diffuse * diff * diffuseColor * (1.0f - shadow);
    vec3 specular = light.color * light.specular * spec * specularColor * (1.0f - shadow);
    return (ambient + diffuse + specular);
}  

vec3 CalculatePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 ambientColor = pow(material.ambient * ambientMultiplier, vec3(1 / gamma));
    vec3 diffuseColor = pow(material.diffuse, vec3(1 / gamma));
    vec3 specularColor = pow(material.specular, vec3(1 / gamma));

    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    float spec;
    if(blinn)
    {
        vec3 halfwayDir = normalize(lightDir + viewDir);  
        spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess * 128);
    }
    else
    {
        vec3 reflectDir = reflect(-lightDir, normal);
        spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess * 128);
    }
    // attenuation
    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // combine results
    vec3 ambient  = light.ambient  * light.color * ambientColor;
    vec3 diffuse  = light.diffuse  * diff * light.color * diffuseColor;
    vec3 specular = light.specular * spec * light.color * specularColor;
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
} 

vec3 CalculateSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 ambientColor = pow(material.ambient * ambientMultiplier, vec3(1 / gamma));
    vec3 diffuseColor = pow(material.diffuse, vec3(1 / gamma));
    vec3 specularColor = pow(material.specular, vec3(1 / gamma));

    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shadingfloat spec;
    float spec;
    if(blinn)
    {
        vec3 halfwayDir = normalize(lightDir + viewDir);  
        spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess * 128);
    }
    else
    {
        vec3 reflectDir = reflect(-lightDir, normal);
        spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess * 128);
    }
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    // combine results
    vec3 ambient  = light.ambient  * light.color * ambientColor;
    vec3 diffuse  = light.diffuse  * diff * light.color * diffuseColor;
    vec3 specular = light.specular * spec * light.color * specularColor;

    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);
}