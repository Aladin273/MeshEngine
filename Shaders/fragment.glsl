#version 330 core
in vec3 FragPos;
in vec3 FragView;
in vec3 Normal;
in vec2 TexCoords;

out vec4 FragColor;

struct Material
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 emission;
    float shininess;
    
    sampler2D diffuseMap;
    sampler2D specularMap;
    sampler2D emissionMap;
}; 

struct DirLight 
{
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight 
{    
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;  
 
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};  

struct SpotLight 
{
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
  
    float constant;
    float linear;
    float quadratic;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;       
};

uniform Material material;

uniform int numDirLights = 0;
uniform int numPointLights = 0;
uniform int numSpotLights = 0;

uniform DirLight dirLights[32];
uniform PointLight pointLights[32];
uniform SpotLight spotLights[32];

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
	
    // Diffuse component
    float diff = max(dot(normal, lightDir), 0.0);
	
    // Specular component
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	
    // Combine
    vec3 ambient = light.ambient * material.ambient * texture(material.diffuseMap, TexCoords).rgb;
    vec3 diffuse = light.diffuse * diff * material.diffuse * texture(material.diffuseMap, TexCoords).rgb;
    vec3 specular = light.specular * spec * material.specular * texture(material.specularMap, TexCoords).rgb;
    vec3 emission = material.emission * texture(material.emissionMap, TexCoords).rgb;
    
    return (ambient + diffuse + specular + emission);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 viewDir, vec3 fragPos)
{
    vec3 lightDir = normalize(light.position - fragPos);
	
    // Diffuse component
    float diff = max(dot(normal, lightDir), 0.0);
	
    // Specular component
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	
    // Attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));   
	
    // Combine
    vec3 ambient = light.ambient * material.ambient * texture(material.diffuseMap, TexCoords).rgb;
    vec3 diffuse = light.diffuse * diff * material.diffuse * texture(material.diffuseMap, TexCoords).rgb;
    vec3 specular = light.specular * spec * material.specular * texture(material.specularMap, TexCoords).rgb;
    vec3 emission = material.emission * texture(material.emissionMap, TexCoords).rgb;
    
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    
    return (ambient + diffuse + specular + emission);
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 viewDir, vec3 fragPos)
{
    vec3 lightDir = normalize(light.position - fragPos);
	
    // Diffuse component
    float diff = max(dot(normal, lightDir), 0.0);
	
    // Specular component
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	
    // Attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
	
    // Intensity
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
	
    // Combine
    vec3 ambient = light.ambient * material.ambient * texture(material.diffuseMap, TexCoords).rgb;
    vec3 diffuse = light.diffuse * diff * material.diffuse * texture(material.diffuseMap, TexCoords).rgb;
    vec3 specular = light.specular * spec * material.specular * texture(material.specularMap, TexCoords).rgb;
    vec3 emission = material.emission * texture(material.emissionMap, TexCoords).rgb;
    
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    
    return (ambient + diffuse + specular + emission);
}

void main()
{    
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(FragView - FragPos);

    vec3 result = vec3(0.0);

    // DirLight
    for(int i = 0; i < numDirLights; ++i)
        result += CalcDirLight(dirLights[i], norm, viewDir);

    // PointLight
    for(int i = 0; i < numPointLights; ++i)
        result += CalcPointLight(pointLights[i], norm, viewDir, FragPos);
    
    // SpotLight
    for(int i = 0; i < numSpotLights; ++i)
        result += CalcSpotLight(spotLights[i], norm, viewDir, FragPos);

    FragColor = vec4(result, 1.0);
}