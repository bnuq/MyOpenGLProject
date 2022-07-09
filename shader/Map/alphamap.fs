#version 460 core

// Vertex Shader => Rasterizer �� ��ģ ��, ������ ����
in vec2 texCoord;
in vec3 WorldNormal;
in vec3 WorldPosition;

// ���ϵǴ� ���� �����׸�Ʈ ����
out vec4 fragColor;

// 0 -> 1 �� �ٲ�鼭 ���´�
uniform float TimeRatio;

void main()
{
    fragColor = vec4(1, 1, 0, 0.5 - TimeRatio * 0.5);
}