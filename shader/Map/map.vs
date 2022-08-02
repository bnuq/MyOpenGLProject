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
    uint story;
};
// SSBO
layout(std430, binding = 1) buffer TileBuffer
{
    Tile tileData[];
};



// �ѹ��� ������ Uniform Variable
uniform vec3 TileScale;         // Ÿ���� ũ�⿡ ���� ����


uniform mat4 transform;         // world -> clip space ��ȯ
uniform mat4 LightTransform;    // world -> �� Ŭ�� �������� ��ȯ



// ���� out
out float IsVisible;            // ���̴� Ÿ�� instance ����, �ƴ� ���� �Ǵ�

out vec3 WorldNormal;
out vec3 WorldPosition;
out vec3 DiffColor;

out vec4 LightClipPos;          // Ÿ�� ������ ����, �� Ŭ�� ���������� ��ǥ


void main()
{
    // ���� ������ ���� Ÿ�� instance �� ���� ����
    Tile curTile = tileData[gl_InstanceID];

    // state �� 0 �� �ƴϴ� => ������ ���°ų�, ����� Ÿ���̴� => GPU Instancing �� ����� �ƴϴ�
    if(curTile.state != 0.0f)
    {
        // Ÿ���� ������, ������ Ŭ�� ���� �� �������� �̵����� => Ÿ���� �׷����� �ʰ� �Ѵ�
        gl_Position = vec4(0, 0, 0, 0);
        
        // ������ �ʴ� Ÿ������ �˸���
        IsVisible = 0.0;
    }
    else
    {
        // ���̴� Ÿ���� �����ϴ� �����̴�
        IsVisible = 1.0;


        // �� ��ȯ => ���� Ÿ���� ��ġ�� �̵��ϰ� �Ѵ�
        // ��¦ �۰� ����� => Ÿ�ϵ��� �ʹ� �پ����� �ʰ� �Ѵ�
        mat4 modelTransform = mat4(
            TileScale.x * 0.95, 0.0, 0.0, 0.0,
            0.0, TileScale.y * 0.95, 0.0, 0.0,
            0.0, 0.0, TileScale.z * 0.95, 0.0,
            curTile.xpos, curTile.ypos, curTile.zpos, 1.0
        );

        // Ŭ�� ���� �� ������ ��ǥ�� ���ϰ�
        gl_Position = transform * modelTransform * vec4(aPos, 1.0);


        // �� ��ֺ��͸� ���� ��ֺ��ͷ� ��ȯ, ������ �𵨺�ȯ ~ ����� ~ ��ġ ����� ����
        WorldNormal = ( transpose(inverse(modelTransform)) * vec4(aNormal, 0.0) ).xyz;
        WorldPosition = (modelTransform * vec4(aPos, 1.0)).xyz;


        // Ÿ���� ���̿� ����, �ٸ� ������ ������ ����. �ϴ� �ִ� 7��, ������ ������ �غ�
        vec3 tempColor = vec3(0, 0, 0);
        switch(curTile.story % 7)
        {
            case 0:     // ��
                tempColor = vec3(1.0, 0.0, 0.0);
                break;

            case 1:     // ��
                tempColor = vec3(1.0, 0.5, 0.0);
                break;

            case 2:     // ��
                tempColor = vec3(1.0, 1.0, 0.0);
                break;

            case 3:     // ��
                tempColor = vec3(0.0, 1.0, 0.0);
                break;

            case 4:     // ��
                tempColor = vec3(0.0, 0.0, 1.0);
                break;

            case 5:     // ��
                tempColor = vec3(0.25, 0.0, 0.5);
                break;

            case 6:     // ��
                tempColor = vec3(0.5, 0.0, 0.5);
                break;
        }

        DiffColor = tempColor;



        // �ش� Ÿ���� ������, ������ Ŭ�� �������� ��ȯ���� �� ��ǥ�� ���Ѵ�
        LightClipPos = LightTransform * vec4(WorldPosition, 1.0);
    }
}