#ifndef __CAM_H__
#define __CAM_H__

#include "common.h"
#include "Character.h"


CLASS_PTR(Camera)
class Camera
{
public:

    glm::vec3 Position;
    glm::vec3 Direction;

    float pitch { 0.0f };         // x �� ���� ȸ��
    

/* Setting Value */
    float xzDist = 10.0f;
    float yDist = 5.0f;
    float rotSpeed { 1.0f };
    glm::vec2 rotLimit { glm::vec2{-10.0f, 30.0f} };


    // ī�޶� ����ٴϴ� �÷��̾�
    CharacterPtr mainChar;



    static CameraPtr Create(CharacterPtr player);
    

    void SetPosition(float);
    void SetDirection();


    void Rotate(float);


private:
    Camera() {}
};

#endif  // __CAM_H__