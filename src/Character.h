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


    // 떨어지고 있는 지 체크
    bool Falling = true;
    // 떨어지고 있지 않을 때, 현재 땅의 높이
    float groundHeight;


/* Setting Values */
    glm::vec3 Acceleration      = glm::vec3(0.0f, -0.001f, 0.0f);
    float JumpPower = 0.05f;
    float MoveSpeed = 0.001f;
    float RotSpeed = 0.1f;


    void Move(int key);
    void Rotate(float);
    void Jump();

    void Fall();
    void Stay(glm::vec3 tempPos);
};


#endif // __CHAR_H__