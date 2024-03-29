#version 460 core


layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;



struct Tile
{
    float xpos;
    float ypos;
    float zpos;
    uint state;
    uint story;
};
// SSBO
layout(std430, binding = 1) buffer TileBuffer
{
    Tile tileData[];
};



// 한번만 들어오는 Uniform Variable
uniform vec3 TileScale;         // 타일의 크기에 대한 정보


uniform mat4 transform;         // world -> clip space 변환
uniform mat4 LightTransform;    // world -> 빛 클립 공간으로 변환



// 공통 out
out float IsVisible;            // 보이는 타일 instance 인지, 아닌 지를 판단

out float curStory;              // 현재 타일의 층수

out vec3 WorldNormal;
out vec3 WorldPosition;
out vec3 DiffColor;

out vec4 LightClipPos;          // 타일 정점에 대한, 빛 클립 공간에서의 좌표


void main()
{
    // 현재 정점이 속한 타일 instance 에 대한 정보
    Tile curTile = tileData[gl_InstanceID];

    // state 가 0 이 아니다 => 반투명 상태거나, 사라진 타일이다 => GPU Instancing 의 대상이 아니다
    if(curTile.state != 0)
    {
        // 타일의 정점을, 무조건 클립 공간 내 원점으로 이동시켜 => 타일이 그려지지 않게 한다
        gl_Position = vec4(0, 0, 0, 0);
        
        // 보이지 않는 타일임을 알린다
        IsVisible = 0.0;
    }
    else
    {
        // 보이는 타일을 구성하는 정점이다
        IsVisible = 1.0;

        curStory = curTile.story;

        // 모델 변환 => 현재 타일의 위치로 이동하게 한다
        // 살짝 작게 만들어 => 타일들이 너무 붙어있지 않게 한다
        mat4 modelTransform = mat4(
            TileScale.x * 0.95, 0.0, 0.0, 0.0,
            0.0, TileScale.y * 0.95, 0.0, 0.0,
            0.0, 0.0, TileScale.z * 0.95, 0.0,
            curTile.xpos, curTile.ypos, curTile.zpos, 1.0
        );

        // 클립 공간 내 정점의 좌표를 구하고
        gl_Position = transform * modelTransform * vec4(aPos, 1.0);


        // 모델 노멀벡터를 월드 노멀벡터로 변환, 원래는 모델변환 ~ 역행렬 ~ 전치 행렬을 적용
        WorldNormal = ( transpose(inverse(modelTransform)) * vec4(aNormal, 0.0) ).xyz;
        WorldPosition = (modelTransform * vec4(aPos, 1.0)).xyz;


        // 타일의 높이에 따라서, 다른 색깔을 가지게 하자. 일단 최대 7개, 무지개 색깔을 준비
        vec3 tempColor = vec3(0, 0, 0);
        switch(curTile.story % 7)
        {
            case 0:     // 빨
                tempColor = vec3(1.0, 0.0, 0.0);
                break;

            case 1:     // 주
                tempColor = vec3(1.0, 0.5, 0.0);
                break;

            case 2:     // 노
                tempColor = vec3(1.0, 1.0, 0.0);
                break;

            case 3:     // 초
                tempColor = vec3(0.0, 1.0, 0.0);
                break;

            case 4:     // 파
                tempColor = vec3(0.0, 0.0, 1.0);
                break;

            case 5:     // 남
                tempColor = vec3(0.25, 0.0, 0.5);
                break;

            case 6:     // 보
                tempColor = vec3(0.5, 0.0, 0.5);
                break;
        }

        DiffColor = tempColor;



        // 해당 타일의 정점을, 광원의 클립 공간으로 변환했을 때 좌표를 구한다
        LightClipPos = LightTransform * vec4(WorldPosition, 1.0);
    }
}