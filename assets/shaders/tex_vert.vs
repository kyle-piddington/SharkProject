#version 330 core
  
layout (location = 0) in vec3 position;
layout (location = 2) in vec2 vertTexCoords;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;
uniform mat3 T = mat3(1,0,0,0,1,0,0,0,1);
out vec2 fragTexCoords;
out vec3 fragPos;

void main()
{
   gl_Position = P * V * M *  vec4(position, 1.0);
   fragTexCoords = (T * vec3(vertTexCoords,0.0)).xy;
   fragPos = position;

}