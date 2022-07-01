#include "Character.h"

/* Rendering ���� */
    // xz ���, y �� ���� �̵��� ��� velocity �� ���Ѵ�
    void Character::Move()
    {
        // �ӵ� �ʱ�ȭ
        auto velocity = glm::vec3(0.0f, 0.0f, 0.0f);
    
        // Ű �Է��� �ް� �ִ�
        if(xzMoving)
        {
            // �ϴ� xz �������� �̵��Ѵ�
            velocity += xzDir * MoveSpeed;
            // ȸ���� �ؾ� �Ѵ�
            Rotate();
        }

        // ���� y �� �������� �̵��Ѵ�
        PrevHeight = Position.y;
        ySpeed -= Gravity;
        velocity.y = ySpeed;


        // �̹��� �뽬 ����� �־��� �� Ȯ��
        if(AddDash)
        {
            // front vec �������� �ӵ��� ���� ���
            velocity += FrontVec * DashPower;
            // ySpeed �� ���� ��´�
            velocity += glm::vec3(0.0f, 1.0f, 0.0f) * DashPower;

            // �ѹ� �߰��ϸ� ���̻� �߰� ����
            AddDash = false;
        }


        // ��ġ�� �̵���Ų��
        Position += velocity;
    }


    // �浹�� ���� ���, y������ ���ϸ� �����
    void Character::yStop()
    {
        ySpeed = 0.0f;
        Position.y = PrevHeight;

        OnGround = true;
        Dashing = false;
        groundHeight = PrevHeight;
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
        else ySpeed += JumpPower;
    }


/* ��� */
    void Character::Dash()
    {
        // �̹� �뽬�� �� ��Ȳ, ���̻��� �뽬�� ������ �ȵȴ�
        if(Dashing)
        {
            return;
        }
        else
        {
            Dashing = true;
            // ���� Move �� Dash �� ���϶�� ���
            AddDash = true;
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
        float ClockOrNot = (CrossRes.y >= 0) ? (+1) : (-1);

        // ���͸� ȸ����ų ���̹Ƿ�, ȸ�� ��ȯ�� �״�� �ٷ� �����ص� ������
        auto YawRot = glm::rotate(glm::mat4(1.0f), glm::radians(ClockOrNot * YawAngleTick), glm::vec3(0.0f, 1.0f, 0.0f));

        FrontVec  = glm::vec3(YawRot * glm::vec4(FrontVec, 0.0f));
        LeftVec   = glm::vec3(YawRot * glm::vec4(LeftVec, 0.0f));
    }