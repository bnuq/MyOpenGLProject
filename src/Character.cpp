#include "Character.h"


void Character::Move()
{
    //camVec.y = 0; // Ȥ�� �𸣴ϱ�
    //Velocity += camVec * MoveSpeed;
    // �ӵ� ���� => ��ӵ� � => ������ �ʴ´�
    // ��ġ �̵����� ����
    //Position += camVec * MoveSpeed;

    // ������ �ִ� �ӵ��� �������� �����δ�
    Position += Velocity * MoveSpeed;
}





void Character::Jump()
{
    SPDLOG_INFO("Jump Count is {}", JumpCount);

    if(JumpCount >= MaxJump) return;
    JumpCount++;
    
    // ������ �Ѵ� = �ӵ��� ��ȭ��Ų��
    // ù��° ������ ����
    if(JumpCount == 1)
    {
        SPDLOG_INFO("JumpCount is 1 and UpVec is {}, {}, {}", UpVec.x, UpVec.y, UpVec.z);
        Velocity += JumpPower * UpVec;
    }
        
    // �ι�° ������ ������
    else
    {
        SPDLOG_INFO("JumpCount is 2 and FrontVec is {}, {}, {}", FrontVec.x, FrontVec.y, FrontVec.z);
        Velocity += JumpPower * FrontVec;
    }
        
}


// �ٴڰ� �浹���� �ʾҴٴ� ���� �̹� ����
void Character::Fall()
{
    // �ϴ� �ӵ��� �ٲ۴�
    Velocity += Acceleration;
    // Ȥ�� xz ��� �� �ӵ��� �ִٸ� �� �ӵ��� ���δ�
    if(glm::dot(Velocity, FrontVec) != 0)
    {
        Velocity -= FrontVec * DashResist;
        if(Velocity.x < 0) Velocity.x = 0;
        if(Velocity.z < 0) Velocity.z = 0;
    }


    // ���� �̵� ��ġ�� ���Ѵ�
    Position += Velocity;

    // ���� ���� �������� ��ġ���� �� ��������, ���� ������ ������ٴ� �� or �ٸ� ������ �����ִ�
    // �������� �ִ� ���·� ������Ʈ �Ѵ�
    if(Position.y < groundHeight)
        Falling = true;
}


// �浹�� ���� ���, ó�����־�� �ϴ� ����
void Character::Stay(glm::vec3 tempPos)
{
    Velocity = glm::vec3(0.0f, 0.0f, 0.0f);
    Position = tempPos;

    // y ������ �������� �ӵ��� ��ġ�� ���ڸ��� ������
    // Velocity.y = 0.0f;
    // Position.y = tempPos.y;

    // ���� Ƚ�� �ʱ�ȭ
    JumpCount = 0;
    // ���̻� �������� ���� ���̴�
    Falling = false;
    // ���� ��ġ�� groundHeight
    groundHeight = tempPos.y;
}



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