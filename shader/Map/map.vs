#version 460 core

// 3개의 attribute
// location = attribute pointer 번호
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;



uniform mat4 transform;         // 모델 => 클립 공간, 변환 행렬
uniform mat4 modelTransform;



out vec3 normal;    // 정점의 월드 좌표계 노멀벡터
out vec2 texCoord;
out vec3 position;  // 정점의 월드 좌표계 좌표






void main() {
    
    gl_Position = transform * vec4(aPos, 1.0);
    texCoord = aTexCoord;

    // 좌표계 변환을 셰이더에서 처리
    // 모델 좌표 => 월드 좌표계 로 노멀벡터 변환               벡터 표시
    normal = (transpose(inverse(modelTransform)) * vec4(aNormal, 0.0)).xyz;
    position = (modelTransform * vec4(aPos, 1.0)).xyz;
}