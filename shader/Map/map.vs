#version 460 core


layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;


struct Tile
{
    float xpos;
    float ypos;
    float zpos;
    float state;
};
// SSBO
layout(std430, binding = 1) buffer TileBuffer
{
    Tile tileData[];
};



// �ѹ��� ������ Uniform Variable
uniform vec3 TileScale;


// world -> clip space ��ȯ
uniform mat4 transform;


out vec3 WorldNormal;
out vec3 WorldPosition;
out vec3 DiffColor;



void main()
{
    // ���� ������ ���� Ÿ�� instance �� ���� ����
    Tile curTile = tileData[gl_InstanceID];

    // state �� 0 �� �ƴϴ� => ������ ���°ų�, ����� Ÿ���̴� => GPU Instancing �� ����� �ƴϴ�
    if(curTile.state != 0.0f)
    {
        // Ÿ���� ������, ������ Ŭ�� ���� �� �������� �̵����� => Ÿ���� �׷����� �ʰ� �Ѵ�
        gl_Position = vec4(0, 0, 0, 0);
    }
    else
    {
        // �� ��ȯ => ���� Ÿ���� ��ġ�� �̵��ϰ� �Ѵ�
        mat4 modelTransform = mat4(
            TileScale.x, 0.0, 0.0, 0.0,
            0.0, TileScale.y, 0.0, 0.0,
            0.0, 0.0, TileScale.z, 0.0,
            curTile.xpos, curTile.ypos, curTile.zpos, 1.0
        );

        // Ŭ�� ���� �� ������ ��ǥ�� ���ϰ�
        gl_Position = transform * modelTransform * vec4(aPos, 1.0);


        // �� ��ֺ��͸� ���� ��ֺ��ͷ� ��ȯ, ������ �𵨺�ȯ ~ ����� ~ ��ġ ����� ����
        WorldNormal = ( transpose(inverse(modelTransform)) * vec4(aNormal, 0.0) ).xyz;
        WorldPosition = (modelTransform * vec4(aPos, 1.0)).xyz;


        // Ÿ���� ���̿� ����, �ٸ� ������ ������ ����. �ϴ� ���⼭�� ������ �����
        DiffColor = vec3(1, 1, 0);
    }
}