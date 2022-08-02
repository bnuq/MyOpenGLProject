#version 460 core


// VAO �� ���� �а� �Ǵ� Vertex Attributes
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
uniform vec3 TileScale;                 // �� ~ Ÿ���� ũ��


uniform mat4 transform;                 // ���� -> Ŭ�� ����
uniform mat4 LightTransform;            // ���� -> �� Ŭ�� ����


uniform uint TileIndex;                 // �̹��� �׸��� ������ Ÿ���� �ε���
uniform float TimeRatio;                // ������� ���� ���� �ð� ����


out vec4 LightClipPos;                  // �� Ŭ�� ���������� ��ǥ
out vec3 TileColor;             // �� ���� ���� �����Ǵ� Ÿ���� �⺻ ����



void main()
{
    // TileIndex �� �Ѱܹ��� Ÿ�� �����Ϳ� �����ؼ�, �׸��� �׸���

    // �� ��ǥ�� => ���� ��ǥ��
    mat4 modelTransform = mat4
    (
        TileScale.x * 0.95, 0.0, 0.0, 0.0,
        0.0, TileScale.y * 0.95, 0.0, 0.0,
        0.0, 0.0, TileScale.z * 0.95, 0.0,
        tileData[TileIndex].xpos, tileData[TileIndex].ypos, tileData[TileIndex].zpos, 1.0
    );


    // �� ��ǥ�� �� ���߾�, (0, 0, 0) ���� ������ ������ �̵��Ѵ�
    // �ð��� �����鼭 ���ݾ� ������� ���� ����
    mat3 localTransform = mat3
    (
        1 - TimeRatio * 0.4, 0, 0,
        0, 1 - TimeRatio * 0.4, 0,
        0, 0, 1 - TimeRatio * 0.4
    );
    // �ð��� ������ ���� �̵��� ����
    vec3 TransApos = localTransform * aPos;


    // �� ��ǥ�迡�� -> ���� ��ǥ�� ��ǥ
    vec4 WorldPosition = modelTransform * vec4(TransApos, 1.0);

        // ���� ��ǥ�� -> Ŭ�� ���� ��ǥ
        gl_Position = transform * WorldPosition;

        // ���� ��ǥ�� -> �� Ŭ�� ���� ��ǥ
        LightClipPos = LightTransform * WorldPosition;

        

    vec3 tempColor = vec3(0, 0, 0);
    switch(tileData[TileIndex].story % 7)
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

    TileColor = tempColor;
}