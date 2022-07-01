#ifndef __CHAR_H__
#define __CHAR_H__

#include "common.h"
#include "Box.h"
#include "Floor.h"


CLASS_PTR(Character)
class Character : public Box
{
public:
    Character(glm::vec3 pos, float x = 1.0f, float y = 1.0f, float z = 1.0f)
    : Box(pos, x, y, z)
    {}

    glm::vec3 FrontVec          = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::vec3 LeftVec          = glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 UpVec             = glm::vec3(0.0f, 1.0f, 0.0f);


    


/* Setting Values */
    
    float DashResist = 0.02f;

    

    
/* Rendering ���� */
    void Move();
    void yStop();
    void OnAir();


/* XZ ��� �̵� ���� */
    glm::vec3 xzDir{};
    bool xzMoving = false;
    float MoveSpeed = 0.01f;

    float YawAngleTick  = 1.0f;

    void GetXZDir(glm::vec3 dir);
    void Rotate();

/* Y �� �̵� ���� */
    float ySpeed = 0.0f;
    float PrevHeight = Position.y;

    // �߷��� �׻� �Ʒ��� �ۿ�, ��� ���� ������ �ϰ� �װ� ���� ������� �۵�
    float Gravity = 0.01f;

    // �������� �ִ� �� üũ
    bool OnGround = false;
    // �������� ���� ���� ��, ���� ���� ����
    float groundHeight;

/* ���� */
    float JumpPower = 0.5f;

/* ��� */
    float DashPower = 0.01f;
    bool Dashing = false;
    bool AddDash = false;

    void Dash();

    




    void Jump();

    void Fall();
    void Stay(glm::vec3 tempPos);




    
};


#endif // __CHAR_H__