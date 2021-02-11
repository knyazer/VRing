#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 color;

out VS_OUT {
    vec3 color;
} vs_out;
 
void main()
{
	vs_out.color = color;
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0); 
}