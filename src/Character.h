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

    

    int JumpCount = 0;
    const int MaxJump = 2;

    glm::vec3 FrontVec          = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::vec3 LeftVec          = glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 UpVec             = glm::vec3(0.0f, 1.0f, 0.0f);


    


/* Setting Values */
    
    float DashResist = 0.02f;

    float JumpPower = 0.05f;

    float MoveSpeed = 0.1f;
    
    float YawAngle      = 0.0f;
    float YawAngleTick  = 10.0f;
    const float YAW_ERROR = 0.001f;


/* XZ 평면 이동 관련 */
    glm::vec3 xzDir{};
    bool xzMoving = false;

/* Y 축 이동 관련 */
    glm::vec3 yVel = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 Acceleration = glm::vec3(0.0f, -0.001f, 0.0f);
    // 떨어지고 있는 지 체크
    bool Falling = true;
    // 떨어지고 있지 않을 때, 현재 땅의 높이
    float groundHeight;

    




    void Jump();

    void Fall();
    void Stay(glm::vec3 tempPos);


/* XZ 평면 이동 관련 */
    void GetXZDir(glm::vec3 dir);
    

    void Rotate();
    void Move();
    
};


#endif // __CHAR_H__