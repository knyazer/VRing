#version 460 core

precision highp float;

layout (points) in;
layout (triangle_strip, max_vertices = 12) out;

uniform mat4 MVP;
uniform vec3 camPos;

vec3 rand(vec3 co)
{
    float r = sin(dot(co.xyz, vec3(12.9898,78.233,23.547))) * 43758.5453;
    return vec3(fract(r), fract(r * 53.123), fract(r / 1.541));
}

in VS_OUT {
    vec3 color;
    int connection;
} gs_in[]; 

flat out vec3 frag;

// Faces coordinates
const ivec3 p[24] = {
        ivec3(1), ivec3(1, 0, 1), ivec3(1, 1, 0), ivec3(1, 0, 0),
        ivec3(0), ivec3(0, 0, 1), ivec3(0, 1, 0), ivec3(0, 1, 1),
        ivec3(1), ivec3(1, 1, 0), ivec3(0, 1, 1), ivec3(0, 1, 0),
        ivec3(0), ivec3(1, 0, 0), ivec3(0, 0, 1), ivec3(1, 0, 1),
        ivec3(1), ivec3(0, 1, 1), ivec3(1, 0, 1), ivec3(0, 0, 1),
        ivec3(0), ivec3(0, 1, 0), ivec3(1, 0, 0), ivec3(1, 1, 0),
};

// Normals for each face
const ivec3 normals[6] = {ivec3(1, 0, 0), ivec3(-1, 0, 0), ivec3(0, 1, 0), ivec3(0, -1, 0), ivec3(0, 0, 1), ivec3(0, 0, -1)};

void main() {
    vec4 pos = gl_in[0].gl_Position;
    vec3 relativePos = pos.xyz - camPos;
    int connection = gs_in[0].connection;

    frag = rand(pos.xyz);


    // For each triangle
    for (int i = 0; i < 24; i++)
    {
        // For each face
        if ((i & 3) == 0)
        {
            // Touch culling
            if (((connection >> (i >> 2)) & 1) == 1)
            {
                i += 3;
                continue;
            }

            // Normal culling
            if (dot(normals[i >> 2], relativePos) > 0)
            {
                i += 3;
                continue;
            }
        }

        // Apply MVP transformation
        gl_Position = MVP * (pos + ivec4(p[i], 0));
        EmitVertex();
        
        // End for each face
        if ((i & 3) == 3)
            EndPrimitive();
    }
}