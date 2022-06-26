#include "common.h"
#include "Player.h"


CLASS_PTR(Camera)
class Camera
{
    glm::vec3 RelativePos { glm::vec3(0.0f, 2.0f, 2.0f) };

    glm::vec3 frontDir { glm::vec3(0.0f, 0.0f, -1.0f) };
    glm::vec3 upDir { glm::vec3(0.0f, 1.0f, 0.0f) };


    float pitch { -20.0f };         // x 축 기준 회전
    float yaw { 0.0f };             // y 축 기준 회전


    float moveSpeed { 0.01f };
    float rotSpeed { 0.01f };


    // 카메라가 따라다니는 플레이어
    PlayerUPtr MainPlayer;    



    // 카메라의 위치를 구하는 함수
    void SetCameraPos();
};