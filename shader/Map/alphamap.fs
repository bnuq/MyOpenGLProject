#version 460 core

in vec3 TileColor;

// ���ϵǴ� ���� �����׸�Ʈ ����
out vec4 fragColor;

// 0 -> 1 �� �ٲ�鼭 ���´�
uniform float TimeRatio;

void main()
{
    fragColor = vec4(TileColor, 0.5 - TimeRatio * 0.5);
}