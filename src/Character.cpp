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

    // y 축 기준 회전변환
    auto YawRot = glm::rotate(glm::mat4(1.0f), glm::radians(DeltaVal), glm::vec3(0.0f, 1.0f, 0.0f));

    // 기존 모델 좌표계를 구성하는 벡터를 회전 시킨다
    FrontVec  = glm::vec3(YawRot * glm::vec4(FrontVec, 0.0f));
    LeftVec   = glm::vec3(YawRot * glm::vec4(LeftVec, 0.0f));
}


void Character::Jump()
{
    if(JumpCount >= MaxJump) return;
    JumpCount++;
    
    // 점프를 한다 = 속도를 변화시킨다
    // 첫번째 점프는 위로
    if(JumpCount == 1)
        Velocity += JumpPower * UpVec;
    // 두번째 점프는 앞으로
    else
        Velocity += JumpPower * FrontVec;
}


// 바닥과 충돌하지 않았다는 것을 이미 전제
void Character::Fall()
{
    // 일단 속도를 바꾼다
    Velocity += Acceleration;    
    // 이후 이동 위치를 구한다
    Position += Velocity;
}


// 충돌을 했을 경우, 처리해주어야 하는 내용
void Character::Stay()
{
    Velocity.y = 0;
    // 점프 횟수 초기화
    JumpCount = 0;
}