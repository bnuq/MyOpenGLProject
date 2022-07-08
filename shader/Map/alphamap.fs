#version 460 core

// Vertex Shader => Rasterizer 를 거친 후, 보간된 값들
in vec2 texCoord;
in vec3 WorldNormal;
in vec3 WorldPosition;

// 리턴되는 최종 프레그먼트 색깔
out vec4 fragColor;


void main()
{
    fragColor = vec4(1, 1, 0, 0.5);
}