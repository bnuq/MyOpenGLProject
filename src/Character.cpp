#include "Character.h"

/* Rendering 과정 */
    // xz 평면, y 축 방향 이동을 모두 velocity 에 더한다
    void Character::Move()
    {
        // 속도 초기화
        auto velocity = glm::vec3(0.0f, 0.0f, 0.0f);
    
        // 키 입력을 받고 있다
        if(xzMoving)
        {
            // 일단 xz 방향으로 이동한다
            velocity += xzDir * MoveSpeed;
            // 회전을 해야 한다
            Rotate();
        }

        // 이후 y 축 방향으로 이동한다
        PrevHeight = Position.y;
        ySpeed -= Gravity;
        velocity.y = ySpeed;


        // 이번에 대쉬 명령이 있었는 지 확인
        if(AddDash)
        {
            // front vec 방향으로 속도를 조금 얻고
            velocity += FrontVec * DashPower;
            // ySpeed 도 조금 얻는다
            velocity += glm::vec3(0.0f, 1.0f, 0.0f) * DashPower;

            // 한번 추가하면 더이상 추가 안함
            AddDash = false;
        }


        // 위치를 이동시킨다
        Position += velocity;
    }


    // 충돌을 했을 경우, y축으로 낙하를 멈춘다
    void Character::yStop()
    {
        ySpeed = 0.0f;
        Position.y = PrevHeight;

        OnGround = true;
        Dashing = false;
        groundHeight = PrevHeight;
    }


    // 충돌이 없는 경우 == 공중에 있다
    void Character::OnAir()
    {
        OnGround = false;
        if(Position.y < groundHeight)
            groundHeight = Position.y;
    }



/* 점프 과정 */
    void Character::Jump()
    {
        // 공중에 있으면 점프 불가능
        if(!OnGround) return;
        else ySpeed += JumpPower;
    }


/* 대시 */
    void Character::Dash()
    {
        // 이미 대쉬를 한 상황, 더이상의 대쉬가 있으면 안된다
        if(Dashing)
        {
            return;
        }
        else
        {
            Dashing = true;
            // 다음 Move 때 Dash 를 더하라고 명령
            AddDash = true;
        }
    }



/* XZ 평면 이동 */

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