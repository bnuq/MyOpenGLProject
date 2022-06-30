#include "Character.h"


void Character::Move()
{
    //camVec.y = 0; // 혹시 모르니까
    //Velocity += camVec * MoveSpeed;
    // 속도 증가 => 등속도 운동 => 멈추질 않는다
    // 위치 이동으로 변경
    //Position += camVec * MoveSpeed;

    // 가지고 있는 속도의 방향으로 움직인다
    Position += Velocity * MoveSpeed;
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
    Velocity += Acceleration;
    // 혹시 xz 평면 위 속도가 있다면 그 속도도 줄인다
    if(glm::dot(Velocity, FrontVec) != 0)
    {
        Velocity -= FrontVec * DashResist;
        if(Velocity.x < 0) Velocity.x = 0;
        if(Velocity.z < 0) Velocity.z = 0;
    }


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
    Velocity = glm::vec3(0.0f, 0.0f, 0.0f);
    Position = tempPos;

    // y 축으로 떨어지는 속도와 위치만 제자리로 돌린다
    // Velocity.y = 0.0f;
    // Position.y = tempPos.y;

    // 점프 횟수 초기화
    JumpCount = 0;
    // 더이상 떨어지지 않을 것이다
    Falling = false;
    // 이전 위치가 groundHeight
    groundHeight = tempPos.y;
}



void Character::GetXZDir(glm::vec3 dir)
{
    xzDir = glm::normalize(xzDir + dir);
    xzMoving = true;
}


// 앞을 바라보는 방향인 front vec 의 방향을 xzDir 방향과 일치시킨다
void Character::Rotate()             
{
    // 혹시 모르니까 정규화 한번 더
    FrontVec.y = 0;
    FrontVec = glm::normalize(FrontVec);

    LeftVec.y = 0;
    LeftVec = glm::normalize(LeftVec);
    
    xzDir.y = 0;
    xzDir = glm::normalize(xzDir);


    auto CrossRes = glm::cross(FrontVec, xzDir);
    
    // 양수 ==> 반시계방향 회전 ==> + 방향으로 회전
    // 음수 ==> 시계방향 회전 ==> - 방향으로 회전
    float ClockOrNot = (CrossRes.y >= 0) ? (+1) : (-1);

    // 벡터를 회전시킬 것이므로, 회전 변환을 그대로 바로 적용해도 괜찮다
    auto YawRot = glm::rotate(glm::mat4(1.0f), glm::radians(ClockOrNot * YawAngleTick), glm::vec3(0.0f, 1.0f, 0.0f));

    FrontVec  = glm::vec3(YawRot * glm::vec4(FrontVec, 0.0f));
    LeftVec   = glm::vec3(YawRot * glm::vec4(LeftVec, 0.0f));
}