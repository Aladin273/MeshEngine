#version 460 core
in vec3 ViewPos;
in vec3 FragPos;
in vec4 FragPosLightSpace;
in vec3 Normal;
in vec2 TexCoords;

out vec4 FragColor;

struct DirLight
{
    vec3 direction;
    float padding1;

    vec3 ambient;
    float padding2;

    vec3 diffuse;
    float padding3;

    vec3 specular;
    float padding4;
};

struct PointLight
{
    vec3 position;
    float padding1;

    float constant;
    float linear;
    float quadratic;
    float padding2;

    vec3 ambient;
    float padding3;

    vec3 diffuse;
    float padding4;

    vec3 specular;
    float padding5;
};

struct SpotLight
{
    vec3 position;
    float padding1;

    vec3 direction;
    float padding2;

    float cutOff;
    float outerCutOff;
    float constant;
    float linear;

    vec3 padding3;
    float quadratic;

    vec3 ambient;
    float padding4;

    vec3 diffuse;
    float padding5;

    vec3 specular;
    float padding6;
};

layout(std140, binding = 1) uniform Lights
{
    int numDirLights;
    int numPointLights;
    int numSpotLights;
    int padding;

    DirLight dirLights[16];
    PointLight pointLights[16];
    SpotLight spotLights[16];
};

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

uniform Material material;

uniform sampler2D depthMap;

float CalcShadow(vec4 fragPosLightSpace)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
	
    float closestDepth = texture(depthMap, projCoords.xy).r; 
    float currentDepth = projCoords.z;
   
    float bias = sqrt(length(ViewPos - FragPos)) * 0.00005;
	
    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(depthMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(depthMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    
    if(projCoords.z > 1.0)
        shadow = 0.0;
        
    return shadow;
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, float shadow)
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
    vec3 specular = light.specular * spec * material.specular * texture(material.specularMap, TexCoords).rrr;
    vec3 emission = material.emission * texture(material.emissionMap, TexCoords).rgb;
    
    return (ambient + (1.0 - shadow) * (diffuse + specular) + emission);
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
    vec3 specular = light.specular * spec * material.specular * texture(material.specularMap, TexCoords).rrr;
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
    vec3 specular = light.specular * spec * material.specular * texture(material.specularMap, TexCoords).rrr;
    vec3 emission = material.emission * texture(material.emissionMap, TexCoords).rgb;
    
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    
    return (ambient + diffuse + specular + emission);
}

void main()
{    
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(ViewPos - FragPos);

    float shadow = 0.0 /*CalcShadow(FragPosLightSpace)*/;

    vec3 result = vec3(0.0);

    // DirLight
    for(int i = 0; i < numDirLights; ++i)
        result += CalcDirLight(dirLights[i], norm, viewDir, shadow);

    // PointLight
    for(int i = 0; i < numPointLights; ++i)
        result += CalcPointLight(pointLights[i], norm, viewDir, FragPos);
    
    // SpotLight
    for(int i = 0; i < numSpotLights; ++i)
        result += CalcSpotLight(spotLights[i], norm, viewDir, FragPos);

    FragColor = vec4(result, 1.0);
}