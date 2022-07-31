#ifndef __CHAR_H__
#define __CHAR_H__

#include "../FrameWork/common.h"

CLASS_PTR(Character)
class Character
{
public:
    Character(glm::vec3 pos, float x = 1.0f, float y = 1.0f, float z = 1.0f)
    : Position(pos), xScale(x), yScale(y), zScale(z)
    {}

    
    glm::vec3 Position;
    
    float xScale;
    float yScale;
    float zScale;

    glm::vec3 FrontVec          = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::vec3 LeftVec           = glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 UpVec             = glm::vec3(0.0f, 1.0f, 0.0f);


    
/* Rendering ���� */
    void Move();
    void yStop();
    void OnAir();

/* XZ ��� �̵� ���� */
    glm::vec3 xzDir{};
    bool xzMoving = false;
    
        float MoveSpeed = 0.8f;
        const float MoveSpeedRatio = 0.1f;

        float YawAngleTick  = 20.0f;
        const float YawAngleTickRatio = 0.1f;

        float YawMinValue = 0.1f;

    void GetXZDir(glm::vec3 dir);
    void Rotate();

/* Y �� �̵� ���� */
    float ySpeed = 0.0f;
    float PrevHeight = Position.y;

        // �߷��� �׻� �Ʒ��� �ۿ�, ��� ���� ������ �ϰ� �װ� ���� ������� �۵�
        float Gravity = 1.0f;
        const float GravityRatio = 0.001f;

    // �������� �ִ� �� üũ
    bool OnGround = false;
    // �������� ���� ���� ��, ���� ���� ����
    float groundHeight = Position.y;

/* ���� */
        float JumpPower = 0.4f;
        const float JumpPowerRatio = 0.1f;

    void Jump();

/* ��� */
        float DashPower = 0.3f;
        const float DashPowerRatio = 0.1f;

    bool Dashing = false;
    bool Waiting = false;
    const double WaitingTime = 1.0;

    void Dash();
    void WaitALittle(double curTime);
};


#endif // __CHAR_H__