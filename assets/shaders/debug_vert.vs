#version 330 core

layout (location = 0) in vec3 position;


uniform mat4 M = mat4(1,0,0,0,
                      0,1,0,0,
                      0,0,1,0,
                      0,0,0,1);
uniform mat4 V;
uniform mat4 P;

void main()
{
   gl_Position = P * V * M *  vec4(position, 1.0);
}