#version 330 core


// VAO �� ���� �а� �Ǵ� Vertex Attributes
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
    // Ŭ�� �������� ������ ��ġ
    gl_Position = transform * vec4(aPos, 1.0);
    // �ؽ�ó ��ǥ, ������� ������
    texCoord = aTexCoord;

    //������ ��ֺ��� => ���� �������� ��ȯ
    WorldNormal = (transpose(inverse(modelTransform)) * vec4(aNormal, 0.0)).xyz;
    // ���� �������� ������ ��ġ
    WorldPosition = (modelTransform * vec4(aPos, 1.0)).xyz;
}