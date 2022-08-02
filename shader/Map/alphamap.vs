#version 460 core


// VAO 에 의해 읽게 되는 Vertex Attributes
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



// 한번만 들어오는 Uniform Variable
uniform vec3 TileScale;                 // 모델 ~ 타일의 크기


uniform mat4 transform;                 // 월드 -> 클립 공간
uniform mat4 LightTransform;            // 월들 -> 빛 클립 공간


uniform uint TileIndex;                 // 이번에 그리는 반투명 타일의 인덱스
uniform float TimeRatio;                // 사라지기 까지 남은 시간 비율


out vec4 LightClipPos;                  // 빛 클립 공간에서의 좌표
out vec3 TileColor;             // 층 수에 따라 결정되는 타일의 기본 색깔



void main()
{
    // TileIndex 로 넘겨받은 타일 데이터에 접근해서, 그림을 그린다

    // 모델 좌표계 => 월드 좌표계
    mat4 modelTransform = mat4
    (
        TileScale.x * 0.95, 0.0, 0.0, 0.0,
        0.0, TileScale.y * 0.95, 0.0, 0.0,
        0.0, 0.0, TileScale.z * 0.95, 0.0,
        tileData[TileIndex].xpos, tileData[TileIndex].ypos, tileData[TileIndex].zpos, 1.0
    );


    // 모델 좌표계 내 정중앙, (0, 0, 0) 으로 점들이 서서히 이동한다
    // 시간이 지나면서 조금씩 사라지는 것을 묘사
    mat3 localTransform = mat3
    (
        1 - TimeRatio * 0.4, 0, 0,
        0, 1 - TimeRatio * 0.4, 0,
        0, 0, 1 - TimeRatio * 0.4
    );
    // 시간이 지남에 따라 이동한 정점
    vec3 TransApos = localTransform * aPos;


    // 모델 좌표계에서 -> 월드 좌표계 좌표
    vec4 WorldPosition = modelTransform * vec4(TransApos, 1.0);

        // 월드 좌표계 -> 클립 공간 좌표
        gl_Position = transform * WorldPosition;

        // 월드 좌표계 -> 빛 클립 공간 좌표
        LightClipPos = LightTransform * WorldPosition;

        

    vec3 tempColor = vec3(0, 0, 0);
    switch(tileData[TileIndex].story % 7)
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

    TileColor = tempColor;
}