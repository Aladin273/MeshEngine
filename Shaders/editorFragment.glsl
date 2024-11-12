#version 330 core

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

uniform Material material;

void main()
{
    FragColor = vec4(material.diffuse + material.emission, 1.0);
}