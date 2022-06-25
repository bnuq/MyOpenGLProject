#version 330 core
out vec4 fragColor;
in vec3 texCoord;

// 2D texture = sampler2D
// Cube Map = samplerCube
uniform samplerCube skybox;

void main() {
    fragColor = texture(skybox, texCoord);
}