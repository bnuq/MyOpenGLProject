#include "Character.h"



// XZ 평면 상 이동 방향을 구하는 함수 <= 입력된 키에 따라서 방향이 추가된다
void Character::GetXZDir(glm::vec3 dir)
{
    xzDir = glm::normalize(xzDir + dir);
    xzMoving = true;
}


// 입력된 입력에 맞춰서 ~ 캐릭터를 이동시키는 함수
// xz 평면, y 축 방향 이동을 모두 velocity 에 더한다
void Character::Move()
{
    // 속도 초기화 <= 매 프레임마다 이동 속도를 새로 구한다
    auto velocity = glm::vec3(0.0f, 0.0f, 0.0f);


    // XZ 평면 상에서 이동 확인, xz 평면 상 이동 키 입력을 받았다면
    if(xzMoving)
    {
        // 일단 xz 방향으로 이동한다
        velocity += xzDir * MoveSpeed * MoveSpeedRatio;

        // 캐릭터가 움직이는 속도 방향으로 회전을 해야 한다
        Rotate();
    }


    
    // 이후 y 축 방향으로 이동계산    
    PrevHeight = Position.y;                        // y 축으로 이동하기 전, 위치를 기억해두고
    ySpeed -= (Gravity * GravityRatio);             // y 축 이동 속도 갱신
    velocity.y = ySpeed;                            // 속도 입력



    // 위치를 이동시킨다
    Position += velocity;
}



/* 타일과 캐릭터 간 충돌을 확인한 다음 */
    // 충돌을 했을 경우, y축상의 이동을 멈춘다
    void Character::yStop()
    {
        ySpeed = 0.0f;                      // y 축 속도 0 으로 초기화
        Position.y = PrevHeight;            // 떨어지기 전 위치로 돌아가고
        OnGround = true;                    // 땅에 닿은 상태임을 알림 => 점프 가능하다
    }

    // 충돌이 없는 경우 == 공중에 있다
    void Character::OnAir()
    {
        OnGround = false;                   // 공중에 있음만 알린다
    }



// 앞을 바라보는 방향인 front vec 의 방향을 현재 움직이고 있는 xzDir 방향과 일치시킨다
void Character::Rotate()             
{
    // 혹시 모르니까 정규화 한번 더
    FrontVec.y = 0;
    FrontVec = glm::normalize(FrontVec);

    LeftVec.y = 0;
    LeftVec = glm::normalize(LeftVec);
    
    xzDir.y = 0;
    xzDir = glm::normalize(xzDir);


    /* 
        FrontVec -> xzDir 과 일치하게 돌려야 한다
        둘 사이의 외적 벡터의 방향을 구하면, 두 벡터를 일치시키기 위한 방향이 시계방향인지 반시계방향인지를 알 수 있다
        
        FrontVec, xzDir 모두 XZ 평면상의 벡터 => 무조건 y 축에 평행한 벡터가 나오므로
        외적 벡터의 y 축 성분이
            양수 ==> 반시계방향 회전 ==> + 방향으로 회전
            음수 ==> 시계방향 회전 ==> - 방향으로 회전

            왜? 외적은 오른손 방향을 따르니까
     */
    float IsCounterClockWise = (glm::cross(FrontVec, xzDir).y >= 0) ? 1.0f : -1.0f;
    

    /* 
        FrontVec 와 xzDir 이 이루는 작은 각은 0 ~ 180 도 사이
        내적을 이용하면 cos(theta) 값은 구할 수 있다
     */
    float CosTheta = glm::dot(FrontVec, xzDir);


    // ( 1 - Cosin Theta ) 값이 일정 이하면, 두 벡터가 충분히 비슷하다는 뜻
    // 그냥 두 벡터를 일치시켜 버리자
    if( (1 - CosTheta) < YawMinValue )
    {
        FrontVec = xzDir;
        LeftVec = glm::cross(UpVec, FrontVec);    // y, z 외적 => x
    }
    // 그 외의 경우, 캐릭터 모델 좌표계를 회전시킨다 => 벡터 회전이므로 회전 변환을 그대로 바로 적용해도 괜찮다
    else
    {
        auto YawRot = glm::rotate(glm::mat4(1.0f), glm::radians(IsCounterClockWise * YawAngleTick * YawAngleTickRatio), glm::vec3(0.0f, 1.0f, 0.0f));

        FrontVec  = glm::vec3(YawRot * glm::vec4(FrontVec, 0.0f));
        LeftVec   = glm::vec3(YawRot * glm::vec4(LeftVec, 0.0f));
    }
}



// 점프 키 입력 시, context 에 의해 바로 호출되는 함수
void Character::Jump()
{
    if(!OnGround)        // 공중에 있으면 점프 불가능
        return;
    else
        ySpeed += JumpPower * JumpPowerRatio;
}