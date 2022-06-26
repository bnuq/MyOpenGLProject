#ifndef __CAM_H__
#define __CAM_H__

#include "common.h"
#include "Player.h"


CLASS_PTR(Camera)
class Camera
{
public:
    float xzDist = 10.0f;
    float yDist = 5.0f;

    glm::vec3 Position;
    glm::vec3 SeeingPoint;


    float pitch { -20.0f };         // x �� ���� ȸ��
    float yaw { 0.0f };             // y �� ���� ȸ��


    float moveSpeed { 0.01f };
    float rotSpeed { 0.01f };


    // ī�޶� ����ٴϴ� �÷��̾�
    PlayerPtr MainPlayer;    



    static CameraPtr Create(PlayerPtr player);
    

    void GetPosition();
    void Rotate(glm::vec2);

    // ī�޶��� ��ġ�� ���ϴ� �Լ�
    void SetCameraPos();


private:
    Camera() {}
};

#endif  // __CAM_H__