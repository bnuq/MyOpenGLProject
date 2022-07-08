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
uniform uint TileCount;
uniform vec3 TileScale;


// Uniform for Vertex Shader
uniform mat4 transform;


out vec2 texCoord;
out vec3 WorldNormal;
out vec3 WorldPosition;
out float state;



void main()
{
    // Ȥ�� ����
    //if(gl_InstanceID >= TileCount) return;



    Tile curTile = tileData[gl_InstanceID];


    if(curTile.state != 0.0f)
    {
        gl_Position = vec4(0, 0, 0, 0);
    }
    else
    {
        // �� ��ȯ
        mat4 modelTransform = mat4(
            TileScale.x, 0.0, 0.0, 0.0,
            0.0, TileScale.y, 0.0, 0.0,
            0.0, 0.0, TileScale.z, 0.0,
            curTile.xpos, curTile.ypos, curTile.zpos, 1.0
        );

        gl_Position = transform * modelTransform * vec4(aPos, 1.0);

        // �� ��ȯ�� �̵��ϰ� �յ� Ȯ��ۿ� �����ϱ�, ��� ���ʹ� �׳� �����ص� �����ϴ�
        WorldNormal = aNormal;
        texCoord = aTexCoord;
        WorldPosition = (modelTransform * vec4(aPos, 1.0)).xyz;
        state = curTile.state;
    }




    
    
    
    // // gl_Position ����
    // // �浹�� Ÿ���̶��, gpu instancing ������ �׳� �׸��� �ʴ´�
    // if(curTile.hasCollision == true)
    // {
    //     gl_Position = vec4(0, 10, 0, 1);
    //     HasCol = 1.0;
    // }
    // else
    // {
        
    // }
}