#version 460 core

// Vertex Shader => Rasterizer 를 거친 후, 보간된 값들
in vec2 texCoord;
in vec3 WorldNormal;
in vec3 WorldPosition;

// 리턴되는 최종 프레그먼트 색깔
out vec4 fragColor;

// 0 -> 1 로 바뀌면서 들어온다
uniform float TimeRatio;

void main()
{
    fragColor = vec4(1, 1, 0, 0.5 - TimeRatio * 0.5);
}