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

    

    
/* Rendering 과정 */
    void Move();
    void yStop();
    void OnAir();


/* XZ 평면 이동 관련 */
    glm::vec3 xzDir{};
    bool xzMoving = false;
    float MoveSpeed = 0.01f;

    float YawAngleTick  = 1.0f;

    void GetXZDir(glm::vec3 dir);
    void Rotate();

/* Y 축 이동 관련 */
    float ySpeed = 0.0f;
    float PrevHeight = Position.y;

    // 중력은 항상 아래로 작용, 양수 값을 가지게 하고 그걸 빼는 방식으로 작동
    float Gravity = 0.01f;

    // 떨어지고 있는 지 체크
    bool OnGround = false;
    // 떨어지고 있지 않을 때, 현재 땅의 높이
    float groundHeight;

/* 점프 */
    float JumpPower = 0.5f;

/* 대시 */
    float DashPower = 0.01f;
    bool Dashing = false;
    bool AddDash = false;

    void Dash();

    




    void Jump();

    void Fall();
    void Stay(glm::vec3 tempPos);




    
};


#endif // __CHAR_H__