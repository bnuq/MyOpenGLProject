#ifndef __CHAR_H__
#define __CHAR_H__

#include "Box.h"
#include "common.h"


class Character : public Box
{
public:
    Character(float x, float y, float z, glm::vec3 pos)
    : Box(x, y, z, pos)
    {}

    glm::vec3 Velocity = glm::vec3(0.0f, 0.0f, 0.0f);

    int JumpCount = 0;
    const int MaxJump = 2;

    glm::vec3 FrontVec          = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::vec3 LeftVec          = glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 UpVec             = glm::vec3(0.0f, 1.0f, 0.0f);


/* Setting Values */
    glm::vec3 Acceleration      = glm::vec3(0.0f, -0.02f, 0.0f);
    float JumpPower = 0.5f;
    float MoveSpeed = 0.1f;
    float RotSpeed = 0.1f;


    void Move(int key);
    void Rotate(float);
    
    void Jump();
    void Fall();
};


#endif // __CHAR_H__