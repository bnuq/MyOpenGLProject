#version 460 core

// ������ ����
uniform vec3 MainColor;


out vec4 fragColor;

void main()
{
    fragColor = vec4(MainColor, 0);
}