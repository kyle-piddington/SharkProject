#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 vertTexCoords;

out vec3 fragPos;
out vec3 fragNor;
out vec2 fragTexCoords;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;
uniform mat3 N;

void main()
{
   vec4 fragPosition = V *  M * vec4(position,1.0);
   fragPos = fragPosition.xyz;
   gl_Position = P * fragPosition;
   fragTexCoords = vertTexCoords;
   fragNor = normalize(N * normal);
}