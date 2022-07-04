#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;


// Uniform for Vertex Shader
uniform mat4 transform;
uniform mat4 modelTransform;
uniform bool Collision;


out vec2 texCoord;
out vec3 WorldNormal;
out vec3 WorldPosition;


void main()
{
    gl_Position = transform * vec4(aPos, 1.0);

    WorldNormal = aNormal;
    texCoord = aTexCoord;
    WorldPosition = (modelTransform * vec4(aPos, 1.0)).xyz;
}