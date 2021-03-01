#version 460 core

vec3 rand(vec3 p)
{
    float r = sin(dot(p.xyz, vec3(12.9898,78.233,23.547))) * 43758.5453;
    return vec3(fract(r), fract(r * 53.123), fract(r / 1.541));
}

layout (location = 0) in float delta;
layout (location = 1) in vec4 aOffset;

out vec3 frag;

uniform mat4 MVP;

const vec3 faces[24] = {
        vec3(1), vec3(1, 0, 1), vec3(1, 1, 0), vec3(1, 0, 0),
        vec3(0), vec3(0, 0, 1), vec3(0, 1, 0), vec3(0, 1, 1),
        vec3(1), vec3(1, 1, 0), vec3(0, 1, 1), vec3(0, 1, 0),
        vec3(0), vec3(1, 0, 0), vec3(0, 0, 1), vec3(1, 0, 1),
        vec3(1), vec3(0, 1, 1), vec3(1, 0, 1), vec3(0, 0, 1),
        vec3(0), vec3(0, 1, 0), vec3(1, 0, 0), vec3(1, 1, 0),
};

void main()
{
    frag = rand(aOffset.xyz);
    gl_Position = MVP * vec4(aOffset.xyz + faces[int(delta + aOffset.w * 4)], 1.0);
}