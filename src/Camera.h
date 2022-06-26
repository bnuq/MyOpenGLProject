#include "common.h"
#include "Player.h"


CLASS_PTR(Camera)
class Camera
{
    glm::vec3 RelativePos { glm::vec3(0.0f, 2.0f, 2.0f) };

    glm::vec3 frontDir { glm::vec3(0.0f, 0.0f, -1.0f) };
    glm::vec3 upDir { glm::vec3(0.0f, 1.0f, 0.0f) };


    float pitch { -20.0f };         // x �� ���� ȸ��
    float yaw { 0.0f };             // y �� ���� ȸ��


    float moveSpeed { 0.01f };
    float rotSpeed { 0.01f };


    // ī�޶� ����ٴϴ� �÷��̾�
    PlayerUPtr MainPlayer;    



    // ī�޶��� ��ġ�� ���ϴ� �Լ�
    void SetCameraPos();
};