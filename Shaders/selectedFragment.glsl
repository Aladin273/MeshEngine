#version 460 core

out vec4 FragColor;

void main()
{
    ivec2 coord = ivec2(gl_FragCoord.xy) / 5;
    FragColor = vec4(1.0, 1.0, 0.0, (coord.x + coord.y) % 2 == 0 ? 0.5 : 0.0);
}