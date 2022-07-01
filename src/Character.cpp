#include "Character.h"
#include <thread>
#include <future>

using namespace std::chrono_literals;

/* Rendering ���� */
    // xz ���, y �� ���� �̵��� ��� velocity �� ���Ѵ�
    void Character::Move()
    {
        // ��ٸ��� ���̶�� �������� �ʴ´�
        if(Waiting)
        {
            // Ȥ�� �ð��� ������ �� Ȯ���Ѵ�
            WaitALittle(glfwGetTime());
            return;
        }


        // �ӵ� �ʱ�ȭ
        auto velocity = glm::vec3(0.0f, 0.0f, 0.0f);

        // ���� ���� �뽬 ������ Ȯ���Ѵ� => �뽬 ���̸� Ű �Է��� ���� �ʴ´�
        if(Dashing)
        {
            // front vec �������� �ӵ��� ��´�
            velocity += FrontVec * DashPower * DashPowerRatio;
        }
        // xz ��� �� �̵� Ű �Է��� �ް� �ִ�
        else if(xzMoving)
        {
            // �ϴ� xz �������� �̵��Ѵ�
            // �̵��ӵ��� 1 / 10
            velocity += xzDir * MoveSpeed * MoveSpeedRatio;
            // ȸ���� �ؾ� �Ѵ�
            Rotate();
        }


        // ���� y �� �������� �̵��Ѵ�
        PrevHeight = Position.y;

        ySpeed -= (Gravity * GravityRatio);
        velocity.y = ySpeed;


        // ��ġ�� �̵���Ų��
        Position += velocity;
    }


    // �浹�� ���� ���, y������ ���ϸ� �����
    void Character::yStop()
    {
        ySpeed = 0.0f;
        Position.y = PrevHeight;

        OnGround = true;
        groundHeight = PrevHeight;

        // �뽬 ���� �ʱ�ȭ
        if(Dashing)
        {
            Dashing = false;
            WaitALittle(glfwGetTime());
        }
    }


    // �浹�� ���� ��� == ���߿� �ִ�
    void Character::OnAir()
    {
        OnGround = false;
        if(Position.y < groundHeight)
            groundHeight = Position.y;
    }



/* ���� ���� */
    void Character::Jump()
    {
        // ���߿� ������ ���� �Ұ���
        if(!OnGround) return;

        // ������ 1 / 10 �� ����
        ySpeed += JumpPower * JumpPowerRatio;
    }


/* ��� */
    void Character::Dash()
    {
        // �̹� �뽬�� �� ��Ȳ, ���̻��� �뽬�� ������ �ȵȴ�
        if(Dashing) return;
        else
        {
            Dashing = true;
            // ������ �����ϰ� ySpeed �� �÷��ش�
            // �������ٴ� �� ���� �پ�� �ϴϱ�, 1 / 10�� �ö���
            ySpeed += DashPower * DashPowerRatio * 0.1f;
        }
    }

    void Character::WaitALittle(double curTime)
    {
        static double storedTime;

        // �ش� ��� �Լ��� ó�� ȣ��Ǵ� ���
        if(Waiting == false)
        {
            Waiting = true;
            // �ð� ����
            storedTime = curTime;
        }
        // �ι�° �̻� ȣ��Ǵ� ��� ==> �ð��� �缭, Waiting �� �ٲپ�� �Ѵ�
        else
        {
            double newTime = curTime;
            if(newTime - storedTime > WaitingTime)
                Waiting = false;
        }
    }



/* XZ ��� �̵� */

    void Character::GetXZDir(glm::vec3 dir)
    {
        xzDir = glm::normalize(xzDir + dir);
        xzMoving = true;
    }


    // ���� �ٶ󺸴� ������ front vec �� ������ xzDir ����� ��ġ��Ų��
    void Character::Rotate()             
    {
        // Ȥ�� �𸣴ϱ� ����ȭ �ѹ� ��
        FrontVec.y = 0;
        FrontVec = glm::normalize(FrontVec);

        LeftVec.y = 0;
        LeftVec = glm::normalize(LeftVec);
        
        xzDir.y = 0;
        xzDir = glm::normalize(xzDir);


        auto CrossRes = glm::cross(FrontVec, xzDir);
        
        // ��� ==> �ݽð���� ȸ�� ==> + �������� ȸ��
        // ���� ==> �ð���� ȸ�� ==> - �������� ȸ��
        // ������ 1 / 10 �� ����
        float ClockOrNot = (CrossRes.y >= 0) ? (+1.0f) : (-1.0f);

        // ���͸� ȸ����ų ���̹Ƿ�, ȸ�� ��ȯ�� �״�� �ٷ� �����ص� ������
        auto YawRot = glm::rotate(glm::mat4(1.0f), glm::radians(ClockOrNot * YawAngleTick * YawAngleTickRatio), glm::vec3(0.0f, 1.0f, 0.0f));

        FrontVec  = glm::vec3(YawRot * glm::vec4(FrontVec, 0.0f));
        LeftVec   = glm::vec3(YawRot * glm::vec4(LeftVec, 0.0f));
    }