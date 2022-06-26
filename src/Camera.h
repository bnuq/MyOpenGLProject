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


    float pitch { -20.0f };         // x 축 기준 회전
    float yaw { 0.0f };             // y 축 기준 회전


    float moveSpeed { 0.01f };
    float rotSpeed { 0.01f };


    // 카메라가 따라다니는 플레이어
    PlayerPtr MainPlayer;    



    static CameraPtr Create(PlayerPtr player);
    

    void GetPosition();
    void Rotate(glm::vec2);

    // 카메라의 위치를 구하는 함수
    void SetCameraPos();


private:
    Camera() {}
};

#endif  // __CAM_H__