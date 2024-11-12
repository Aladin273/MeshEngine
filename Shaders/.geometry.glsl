#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in vec3 FragPos[];
in vec3 Normal[];
in vec2 TexCoords[];
in vec4 FragPosLightSpace[];
in vec3 ViewPos[];

out vec3 gFragPos;
out vec3 gNormal;
out vec2 gTexCoords;
out vec4 gFragPosLightSpace;
out vec3 gViewPos;

uniform bool flatShading;

void main() 
{
    for (int i = 0; i < 3; ++i) 
    {
        gFragPos = FragPos[i];
        gTexCoords = TexCoords[i];
        gFragPosLightSpace = FragPosLightSpace[i];
        gViewPos = ViewPos[i];
        
        if (flatShading)
        {
            gNormal = normalize(cross(FragPos[1] - FragPos[0], FragPos[2] - FragPos[0]));;
        }
        else
        {
            gNormal = Normal[i];
        }

        gl_Position = gl_in[i].gl_Position;
        EmitVertex();
    }
    EndPrimitive();
}
