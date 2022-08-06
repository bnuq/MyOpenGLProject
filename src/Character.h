#ifndef __CHAR_H__
#define __CHAR_H__

#include "../FrameWork/common.h"

// �÷��̾ �����ϴ� ĳ����
CLASS_PTR(Character)
class Character
{
public:
    // ��ġ
    glm::vec3 Position;

    // ĳ���Ͱ� ���� ������ �����ϴ� �ٴ�
    unsigned int CharStory = 0;
    
    // ĳ���ʹ� Box ���¸� ������ => �� ���⺰ scale �� ����
    float xScale;
    float yScale;
    float zScale;

    // ĳ������ �� ��ǥ��
    glm::vec3 FrontVec          = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::vec3 LeftVec           = glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 UpVec             = glm::vec3(0.0f, 1.0f, 0.0f);



    // Constructor 
    Character(glm::vec3 pos, float x = 1.0f, float y = 1.0f, float z = 1.0f)
    : Position(pos), xScale(x), yScale(y), zScale(z)
    {}


    // Ű �Է� => XZ ��鿡���� �̵� ���ο� �̵� ������ ����
    void GetXZDir(glm::vec3 dir);
        glm::vec3 xzDir{};
        bool xzMoving = false;


    // Ű �Է¿� ���� ������
    void Move();
        /* XZ ��� �̵� ���� */
        float MoveSpeed = 1.5f;
        const float MoveSpeedRatio = 0.1f;

        /* Y �� �̵� ���� */
        float ySpeed = 0.0f;                    // ���� y �� ���� �̵��ӵ�
        float PrevHeight;

        float Gravity = 1.0f;                   // �߷��� �׻� �Ʒ��� �ۿ�, ��� ���� ������ �ϰ� �װ� ���� ������� �۵�
        const float GravityRatio = 0.001f;


    // y �� ���� ������
    void yStop();
    void OnAir();


    // ĳ���Ͱ� �����̴� ���⿡ �°�, ĳ���͸� ȸ��
    void Rotate();
        float YawAngleTick  = 1.0f;
        const float YawAngleTickRatio = 1.0f;
        float YawMinValue = 0.01f;


    // ���� ó��
    void Jump();
        bool OnGround = false;                  // �������� �ִ� �� üũ => ���� ������ ���µ� ���
        float JumpPower = 1.0f;
        const float JumpPowerRatio = 0.1f;
};


#endif // __CHAR_H__