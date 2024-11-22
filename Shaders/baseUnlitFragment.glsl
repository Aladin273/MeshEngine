#version 460 core

in vec3 ViewPos;
in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

out vec4 FragColor;

struct Material
{
    vec3 ambient;
    vec4 diffuse;
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
    float alpha = material.diffuse.a * texture(material.diffuseMap, TexCoords).a;
    
    if (alpha < 0.1f) 
        discard;

    vec3 diffuse = material.diffuse.rgb * texture(material.diffuseMap, TexCoords).rgb;
    vec3 emission = material.emission * texture(material.emissionMap, TexCoords).rgb;

    FragColor = vec4(diffuse + emission, 0.0);
}