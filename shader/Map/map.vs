#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aOffset;
layout (location = 4) in vec2 aBool;


struct FloorStr
{
    vec3 FloorPos;
    int FloorTouched;
};


layout (std430, binding = 8) buffer FloorBuffer
{
    FloorStr floorstr[];
};




uniform mat4 transform;

out vec3 normal;
out vec2 texCoord;
out vec3 position;
out vec2 bBool;

int instanceID;

void main() {
    gl_Position = transform * vec4(aPos + floorstr[gl_InstanceID].FloorPos, 1.0f);
    //gl_Position = transform * vec4(aPos + aOffset, 1.0);
    texCoord = aTexCoord;

    normal = aNormal;
    position = aPos + aOffset;
    bBool = aBool;
    instanceID = gl_InstanceID;
}