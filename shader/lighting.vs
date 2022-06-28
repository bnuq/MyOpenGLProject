#version 460 core


layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;



uniform mat4 transform;
uniform mat4 modelTransform;



out vec3 normal;
out vec2 texCoord;
out vec3 position;






void main() {
    
    gl_Position = transform * vec4(aPos, 1.0);
    texCoord = aTexCoord;

    // ì¢Œí‘œê³? ë³??™˜?„ ?…°?´?”?—?„œ ì²˜ë¦¬
    // ëª¨ë¸ ì¢Œí‘œ => ?›”?“œ ì¢Œí‘œê³? ë¡? ?…¸ë©?ë²¡í„° ë³??™˜               ë²¡í„° ?‘œ?‹œ
    normal = (transpose(inverse(modelTransform)) * vec4(aNormal, 0.0)).xyz;
    position = (modelTransform * vec4(aPos, 1.0)).xyz;
}