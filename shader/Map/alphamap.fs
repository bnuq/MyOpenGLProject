#version 460 core

// Vertex Shader => Rasterizer �� ��ģ ��, ������ ����
in vec2 texCoord;
in vec3 WorldNormal;
in vec3 WorldPosition;

// ���ϵǴ� ���� �����׸�Ʈ ����
out vec4 fragColor;


void main()
{
    fragColor = vec4(1, 1, 0, 0.5);
}