#version 460 core

layout (location = 0) in ivec3 aPos;
//layout (location = 1) in vec3 color;
layout (location = 2) in int connection;

out VS_OUT {
    vec3 color;
    int connection;
} vs_out;
 
void main()
{
	vs_out.color = vec3(0.4);
    vs_out.connection = connection;
    gl_Position = ivec4(aPos.x, aPos.y, aPos.z, 1.0); 
}