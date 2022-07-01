#include "Character.h"
#include <thread>
#include <future>

using namespace std::chrono_literals;

/* Rendering 과정 */
    // xz 평면, y 축 방향 이동을 모두 velocity 에 더한다
    void Character::Move()
    {
        // 기다리는 중이라면 움직이지 않는다
        if(Waiting)
        {
            // 혹시 시간이 지났는 지 확인한다
            WaitALittle(glfwGetTime());
            return;
        }


        // 속도 초기화
        auto velocity = glm::vec3(0.0f, 0.0f, 0.0f);

        // 가장 먼저 대쉬 중인지 확인한다 => 대쉬 중이면 키 입력을 받지 않는다
        if(Dashing)
        {
            // front vec 방향으로 속도를 얻는다
            velocity += FrontVec * DashPower * DashPowerRatio;
        }
        // xz 평면 상 이동 키 입력을 받고 있다
        else if(xzMoving)
        {
            // 일단 xz 방향으로 이동한다
            // 이동속도는 1 / 10
            velocity += xzDir * MoveSpeed * MoveSpeedRatio;
            // 회전을 해야 한다
            Rotate();
        }


        // 이후 y 축 방향으로 이동한다
        PrevHeight = Position.y;

        ySpeed -= (Gravity * GravityRatio);
        velocity.y = ySpeed;


        // 위치를 이동시킨다
        Position += velocity;
    }


    // 충돌을 했을 경우, y축으로 낙하를 멈춘다
    void Character::yStop()
    {
        ySpeed = 0.0f;
        Position.y = PrevHeight;

        OnGround = true;
        groundHeight = PrevHeight;

        // 대쉬 상태 초기화
        if(Dashing)
        {
            Dashing = false;
            WaitALittle(glfwGetTime());
        }
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

        // 점프는 1 / 10 만 적용
        ySpeed += JumpPower * JumpPowerRatio;
    }


/* 대시 */
    void Character::Dash()
    {
        // 이미 대쉬를 한 상황, 더이상의 대쉬가 있으면 안된다
        if(Dashing) return;
        else
        {
            Dashing = true;
            // 점프와 동일하게 ySpeed 를 올려준다
            // 점프보다는 덜 높게 뛰어야 하니까, 1 / 10만 올라가자
            ySpeed += DashPower * DashPowerRatio * 0.1f;
        }
    }

    void Character::WaitALittle(double curTime)
    {
        static double storedTime;

        // 해당 멤버 함수가 처음 호출되는 경우
        if(Waiting == false)
        {
            Waiting = true;
            // 시간 갱신
            storedTime = curTime;
        }
        // 두번째 이상 호출되는 경우 ==> 시간을 재서, Waiting 을 바꾸어야 한다
        else
        {
            double newTime = curTime;
            if(newTime - storedTime > WaitingTime)
                Waiting = false;
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
        // 각도는 1 / 10 만 적용
        float ClockOrNot = (CrossRes.y >= 0) ? (+1.0f) : (-1.0f);

        // 벡터를 회전시킬 것이므로, 회전 변환을 그대로 바로 적용해도 괜찮다
        auto YawRot = glm::rotate(glm::mat4(1.0f), glm::radians(ClockOrNot * YawAngleTick * YawAngleTickRatio), glm::vec3(0.0f, 1.0f, 0.0f));

        FrontVec  = glm::vec3(YawRot * glm::vec4(FrontVec, 0.0f));
        LeftVec   = glm::vec3(YawRot * glm::vec4(LeftVec, 0.0f));
    }