#version 460 core

in vec3 TileColor;

// 리턴되는 최종 프레그먼트 색깔
out vec4 fragColor;

// 0 -> 1 로 바뀌면서 들어온다
uniform float TimeRatio;

void main()
{
    // 사라지는 순간 반투명이 갑자기 사라지게 하고 싶다 => 기본 알파값을 줘서, 있는 동안은 불투명한게 보이도록
    fragColor = vec4(TileColor, 0.5 - TimeRatio * 0.5 + 0.2);
}