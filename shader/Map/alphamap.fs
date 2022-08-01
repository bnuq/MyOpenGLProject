#version 460 core

in vec3 TileColor;

// 리턴되는 최종 프레그먼트 색깔
out vec4 fragColor;

// 0 -> 1 로 바뀌면서 들어온다
uniform float TimeRatio;

void main()
{
    fragColor = vec4(TileColor, 0.5 - TimeRatio * 0.5);
}