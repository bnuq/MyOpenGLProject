#include "Character.h"


void Character::Move(int key)
{
    switch (key)
    {
    case GLFW_KEY_W:
        Position += MoveSpeed * FrontVec;
        break;
    
    case GLFW_KEY_S:
        Position -= MoveSpeed * FrontVec;
        break;

    case GLFW_KEY_D:
        Position -= MoveSpeed * LeftVec;
        break;

    case GLFW_KEY_A:
        Position += MoveSpeed * LeftVec;
        break;

    default:
        break;
    }
}


void Character::Rotate(float angle)
{
    float DeltaVal = -angle * RotSpeed;
    auto YawRot = glm::rotate(glm::mat4(1.0f), glm::radians(DeltaVal), glm::vec3(0.0f, 1.0f, 0.0f));

    FrontVec  = glm::vec3(YawRot * glm::vec4(FrontVec, 0.0f));
    LeftVec   = glm::vec3(YawRot * glm::vec4(LeftVec, 0.0f));
}


void Character::Jump()
{
    if(JumpCount >= MaxJump) return;
    JumpCount++;
    
    Velocity += JumpPower * UpVec;
}


void Character::Fall()
{
    
}