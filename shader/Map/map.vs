#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;


struct Tile
{
    /* 
        xyz = Position
    */
    vec4 position;

    /* 
        x   = collision == 0 이면, 충돌이 없었다
        y   = disappear == 0 이면, 안 사라진다
        z   = save time ==> 시간 저장
    */
    vec4 collAndTime;
};
// SSBO
layout(std430, binding = 1) buffer TileBuffer
{
    Tile tileData[];
};


// 한번만 들어오는 Uniform Variable
uniform uint TileCount;
uniform vec3 TileScale;


// Uniform for Vertex Shader
uniform mat4 transform;


out vec2 texCoord;
out vec3 WorldNormal;
out vec3 WorldPosition;
out float col;
out float dis;


void main()
{
    // 혹시 몰라서
    if(gl_InstanceID >= TileCount) return;

    Tile curTile = tileData[gl_InstanceID];
    
    mat4 modelTransform = mat4(
        TileScale.x, 0.0, 0.0, 0.0,
        0.0, TileScale.y, 0.0, 0.0,
        0.0, 0.0, TileScale.z, 0.0,
        curTile.position.x, curTile.position.y, curTile.position.z, 1.0
    );

    gl_Position = transform * modelTransform * vec4(aPos, 1.0);

    
    // 모델 변환이 이동하고 균등 확대밖에 없으니까, 노멀 벡터는 그냥 진행해도 무관하다
    WorldNormal = aNormal;


    texCoord = aTexCoord;


    WorldPosition = (modelTransform * vec4(aPos, 1.0)).xyz;

    col = curTile.collAndTime.x;
    dis = curTile.collAndTime.y;
}