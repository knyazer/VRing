#version 330 core

precision highp float;

layout (points) in;
layout (triangle_strip, max_vertices = 36) out;
uniform mat4 MVP;

vec3 rand(vec3 co)
{
    float r = sin(dot(co.xyz ,vec3(12.9898,78.233,23.547))) * 43758.5453;
    return vec3(fract(r), fract(r * 53.123), fract(r / 1.541));
}

in VS_OUT {
    vec3 color;
} gs_in[]; 

out vec3 frag;

const vec3 p[24] = {
        vec3(0),  vec3(0, 1, 0),vec3(1, 0, 0), vec3(1, 1, 0),
        vec3(0), vec3(0, 0, 1), vec3(0, 1, 0), vec3(0, 1, 1),
        vec3(0), vec3(1, 0, 0), vec3(0, 0, 1), vec3(1, 0, 1),
        vec3(1), vec3(0, 1, 1), vec3(1, 0, 1), vec3(0, 0, 1),
        vec3(1), vec3(1, 0, 1), vec3(1, 1, 0), vec3(1, 0, 0),
        vec3(1), vec3(1, 1, 0), vec3(0, 1, 1), vec3(0, 1, 0)
};

void main() {
    vec4 pos = gl_in[0].gl_Position;
    

    frag = rand(pos.xyz);


    for (int i = 0; i < 24; i++)
    {
        frag = rand(frag);
        gl_Position = MVP * (pos + vec4(p[i], 0));
        EmitVertex();
        
        if ((i & 3) == 3)
            EndPrimitive();
    }
}    