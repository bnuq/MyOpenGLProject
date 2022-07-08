#version 460 core


// VAO 에 의해 읽게 되는 Vertex Attributes
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;


uniform mat4 transform;
uniform vec3 TilePos;
uniform vec3 TileScale;


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



    // 클립 공간에서 정점의 위치
    gl_Position = transform * modelTransform * vec4(aPos, 1.0);
    // 텍스처 좌표, 변경없이 가져옴
    texCoord = aTexCoord;

    //정점의 노멀벡터 => 월드 공간으로 변환
    WorldNormal = (transpose(inverse(modelTransform)) * vec4(aNormal, 0.0)).xyz;
    // 월드 공간에서 정점의 위치
    WorldPosition = (modelTransform * vec4(aPos, 1.0)).xyz;
}