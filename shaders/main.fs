#version 460 core

in vec3 frag;
out vec4 FragColor;

void main()
{
    FragColor = vec4(frag.xyz, 1.0);   
}