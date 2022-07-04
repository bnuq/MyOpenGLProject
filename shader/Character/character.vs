#version 330 core


// VAO 에 의해 읽게 되는 Vertex Attributes
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;


uniform mat4 transform;
uniform mat4 modelTransform;

out vec2 texCoord;
out vec3 WorldNormal;
out vec3 WorldPosition;

void main()
{
    // 클립 공간에서 정점의 위치
    gl_Position = transform * vec4(aPos, 1.0);
    // 텍스처 좌표, 변경없이 가져옴
    texCoord = aTexCoord;

    //정점의 노멀벡터 => 월드 공간으로 변환
    WorldNormal = (transpose(inverse(modelTransform)) * vec4(aNormal, 0.0)).xyz;
    // 월드 공간에서 정점의 위치
    WorldPosition = (modelTransform * vec4(aPos, 1.0)).xyz;
}