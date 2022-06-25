#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 transform;

void main() {
    // MVP 변환 => 클립 공간 좌표로 변환
    gl_Position = transform * vec4(aPos, 1.0);
}