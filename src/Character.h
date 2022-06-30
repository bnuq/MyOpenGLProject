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

    glm::vec3 Velocity = glm::vec3(0.0f, 0.0f, 0.0f);

    int JumpCount = 0;
    const int MaxJump = 2;

    glm::vec3 FrontVec          = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::vec3 LeftVec          = glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 UpVec             = glm::vec3(0.0f, 1.0f, 0.0f);


    // �������� �ִ� �� üũ
    bool Falling = true;
    // �������� ���� ���� ��, ���� ���� ����
    float groundHeight;


/* Setting Values */
    glm::vec3 Acceleration      = glm::vec3(0.0f, -0.001f, 0.0f);
    float DashResist = 0.02f;

    float JumpPower = 0.05f;

    float MoveSpeed = 0.01f;
    
    float YawAngle      = 0.0f;
    float YawAngleTick  = 0.4f;
    const float YAW_ERROR = 0.001f;


    void Move(glm::vec3 camVec);           
    void Rotate(glm::vec3 camVec);
    void Jump();

    void Fall();
    void Stay(glm::vec3 tempPos);
};


#endif // __CHAR_H__