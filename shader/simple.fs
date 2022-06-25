#version 330 core

uniform vec4 color;
//uniform vec4 anoColor;

out vec4 fragColor;

void main() {
    fragColor = color;
    //fragColor = anoColor;
}