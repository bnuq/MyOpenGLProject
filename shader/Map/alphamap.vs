#version 460 core


// VAO 에 의해 읽게 되는 Vertex Attributes
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;


uniform mat4 transform;
uniform vec3 TilePos;
uniform vec3 TileScale;

uniform float TimeRatio;


out vec2 texCoord;
out vec3 WorldNormal;
out vec3 WorldPosition;

void main()
{
    mat4 modelTransform = mat4
        (
            TileScale.x, 0.0, 0.0, 0.0,
            0.0, TileScale.y, 0.0, 0.0,
            0.0, 0.0, TileScale.z, 0.0,
            TilePos.x, TilePos.y, TilePos.z, 1.0
        );

    // 월드 공간에서 정점의 위치
    WorldPosition = (modelTransform * vec4(aPos, 1.0)).xyz;

    // 텍스처 좌표, 변경없이 가져옴
    texCoord = aTexCoord;

    //정점의 노멀벡터 => 월드 공간으로 변환
    WorldNormal = (transpose(inverse(modelTransform)) * vec4(aNormal, 0.0)).xyz;


    // 모델 좌표계 내에서, 점들을 가운데, (0, 0, 0) 으로 이동시킨다
    // 시간이 지나면서 조금씩 사라지는 것을 묘사
    
    mat3 localTransform = mat3
    (
        1 - TimeRatio * 0.2, 0, 0,
        0, 1 - TimeRatio * 0.2, 0,
        0, 0, 1 - TimeRatio * 0.2
    );
    
    vec3 TransApos = localTransform * aPos;


    // 클립 공간에서 정점의 위치
    gl_Position = transform * modelTransform * vec4(TransApos, 1.0);
}