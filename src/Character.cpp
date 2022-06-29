#include "Character.h"


void Character::Move(glm::vec3 camVec)
{
    camVec.y = 0; // 혹시 모르니까
    //Velocity += camVec * MoveSpeed;
    // 속도 증가 => 등속도 운동 => 멈추질 않는다
    // 위치 이동으로 변경
    Position += camVec * MoveSpeed;
}


void Character::Rotate(glm::vec3 camVec)             
{
    FrontVec = glm::normalize(FrontVec);
    camVec = glm::normalize(camVec);
             
    auto CrossRes = glm::cross(FrontVec, camVec);
    float RotAngle = 0;

    // 두 벡터가 거의 일치하는 상황
    // if(CrossRes.y > -YAW_ERROR && CrossRes.y < YAW_ERROR)
    // {
    //     // 회전할 게 없다 => front vec 를 완전히 일치시켜 버린다
    //     FrontVec = camVec;
    //     return;
    // }
    // 양수 => 반시계방향 회전의 경우
    if(CrossRes.y >= 0)
    {
        // 모델 좌표계 전체를 회전시켜야 한다, 위치는 바뀌지 않는다
        // 그럼 그냥 벡터니까 회전 적용하면 될듯?
        auto YawRot = glm::rotate(glm::mat4(1.0f), glm::radians(YawAngleTick), glm::vec3(0.0f, 1.0f, 0.0f));

        // 기존 모델 좌표계를 구성하는 벡터를 회전 시킨다
        FrontVec  = glm::vec3(YawRot * glm::vec4(FrontVec, 0.0f));
        LeftVec   = glm::vec3(YawRot * glm::vec4(LeftVec, 0.0f));
    }
    // 음수 => 시계방향 회전
    else
    {
        auto YawRot = glm::rotate(glm::mat4(1.0f), glm::radians(-YawAngleTick), glm::vec3(0.0f, 1.0f, 0.0f));

        // 기존 모델 좌표계를 구성하는 벡터를 회전 시킨다
        FrontVec  = glm::vec3(YawRot * glm::vec4(FrontVec, 0.0f));
        LeftVec   = glm::vec3(YawRot * glm::vec4(LeftVec, 0.0f));
    }
}


void Character::Jump()
{
    SPDLOG_INFO("Jump Count is {}", JumpCount);

    if(JumpCount >= MaxJump) return;
    JumpCount++;
    
    // 점프를 한다 = 속도를 변화시킨다
    // 첫번째 점프는 위로
    if(JumpCount == 1)
    {
        SPDLOG_INFO("JumpCount is 1 and UpVec is {}, {}, {}", UpVec.x, UpVec.y, UpVec.z);
        Velocity += JumpPower * UpVec;
    }
        
    // 두번째 점프는 앞으로
    else
    {
        SPDLOG_INFO("JumpCount is 2 and FrontVec is {}, {}, {}", FrontVec.x, FrontVec.y, FrontVec.z);
        Velocity += JumpPower * FrontVec;
    }
        
}


// 바닥과 충돌하지 않았다는 것을 이미 전제
void Character::Fall()
{
    // 일단 속도를 바꾼다
    Velocity += Acceleration * 0.05f;
    // 이후 이동 위치를 구한다
    Position += Velocity;

    // 만약 기존 떨어지던 위치보다 더 내려가면, 기존 발판이 사라졌다는 뜻 or 다른 레벨로 가고있다
    // 떨어지고 있는 상태로 업데이트 한다
    if(Position.y < groundHeight)
        Falling = true;
}


// 충돌을 했을 경우, 처리해주어야 하는 내용
void Character::Stay(glm::vec3 tempPos)
{
    //Velocity = glm::vec3(0.0f, 0.0f, 0.0f);
    //Position = tempPos;

    // y 축으로 떨어지는 속도와 위치만 제자리로 돌린다
    Velocity.y = 0.0f;
    Position.y = tempPos.y;

    // 점프 횟수 초기화
    JumpCount = 0;
    // 더이상 떨어지지 않을 것이다
    Falling = false;
    // 이전 위치가 groundHeight
    groundHeight = tempPos.y;
}