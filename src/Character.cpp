#include "Character.h"


void Character::Move(glm::vec3 camVec)
{
    camVec.y = 0; // Ȥ�� �𸣴ϱ�
    //Velocity += camVec * MoveSpeed;
    // �ӵ� ���� => ��ӵ� � => ������ �ʴ´�
    // ��ġ �̵����� ����
    Position += camVec * MoveSpeed;
}


void Character::Rotate(glm::vec3 camVec)             
{
    FrontVec = glm::normalize(FrontVec);
    camVec = glm::normalize(camVec);
             
    auto CrossRes = glm::cross(FrontVec, camVec);
    float RotAngle = 0;

    // �� ���Ͱ� ���� ��ġ�ϴ� ��Ȳ
    // if(CrossRes.y > -YAW_ERROR && CrossRes.y < YAW_ERROR)
    // {
    //     // ȸ���� �� ���� => front vec �� ������ ��ġ���� ������
    //     FrontVec = camVec;
    //     return;
    // }
    // ��� => �ݽð���� ȸ���� ���
    if(CrossRes.y >= 0)
    {
        // �� ��ǥ�� ��ü�� ȸ�����Ѿ� �Ѵ�, ��ġ�� �ٲ��� �ʴ´�
        // �׷� �׳� ���ʹϱ� ȸ�� �����ϸ� �ɵ�?
        auto YawRot = glm::rotate(glm::mat4(1.0f), glm::radians(YawAngleTick), glm::vec3(0.0f, 1.0f, 0.0f));

        // ���� �� ��ǥ�踦 �����ϴ� ���͸� ȸ�� ��Ų��
        FrontVec  = glm::vec3(YawRot * glm::vec4(FrontVec, 0.0f));
        LeftVec   = glm::vec3(YawRot * glm::vec4(LeftVec, 0.0f));
    }
    // ���� => �ð���� ȸ��
    else
    {
        auto YawRot = glm::rotate(glm::mat4(1.0f), glm::radians(-YawAngleTick), glm::vec3(0.0f, 1.0f, 0.0f));

        // ���� �� ��ǥ�踦 �����ϴ� ���͸� ȸ�� ��Ų��
        FrontVec  = glm::vec3(YawRot * glm::vec4(FrontVec, 0.0f));
        LeftVec   = glm::vec3(YawRot * glm::vec4(LeftVec, 0.0f));
    }
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
    Velocity += Acceleration * 0.05f;
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
    //Velocity = glm::vec3(0.0f, 0.0f, 0.0f);
    //Position = tempPos;

    // y ������ �������� �ӵ��� ��ġ�� ���ڸ��� ������
    Velocity.y = 0.0f;
    Position.y = tempPos.y;

    // ���� Ƚ�� �ʱ�ȭ
    JumpCount = 0;
    // ���̻� �������� ���� ���̴�
    Falling = false;
    // ���� ��ġ�� groundHeight
    groundHeight = tempPos.y;
}