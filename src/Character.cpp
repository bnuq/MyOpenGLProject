#include "Character.h"



// XZ ��� �� �̵� ������ ���ϴ� �Լ� <= �Էµ� Ű�� ���� ������ �߰��ȴ�
void Character::GetXZDir(glm::vec3 dir)
{
    xzDir = glm::normalize(xzDir + dir);
    xzMoving = true;
}


// �Էµ� �Է¿� ���缭 ~ ĳ���͸� �̵���Ű�� �Լ�
// xz ���, y �� ���� �̵��� ��� velocity �� ���Ѵ�
void Character::Move()
{
    // �ӵ� �ʱ�ȭ <= �� �����Ӹ��� �̵� �ӵ��� ���� ���Ѵ�
    auto velocity = glm::vec3(0.0f, 0.0f, 0.0f);


    // XZ ��� �󿡼� �̵� Ȯ��, xz ��� �� �̵� Ű �Է��� �޾Ҵٸ�
    if(xzMoving)
    {
        // �ϴ� xz �������� �̵��Ѵ�
        velocity += xzDir * MoveSpeed * MoveSpeedRatio;

        // ĳ���Ͱ� �����̴� �ӵ� �������� ȸ���� �ؾ� �Ѵ�
        Rotate();
    }


    
    // ���� y �� �������� �̵����    
    PrevHeight = Position.y;                        // y ������ �̵��ϱ� ��, ��ġ�� ����صΰ�
    ySpeed -= (Gravity * GravityRatio);             // y �� �̵� �ӵ� ����
    velocity.y = ySpeed;                            // �ӵ� �Է�



    // ��ġ�� �̵���Ų��
    Position += velocity;
}



/* Ÿ�ϰ� ĳ���� �� �浹�� Ȯ���� ���� */
    // �浹�� ���� ���, y����� �̵��� �����
    void Character::yStop()
    {
        ySpeed = 0.0f;                      // y �� �ӵ� 0 ���� �ʱ�ȭ
        Position.y = PrevHeight;            // �������� �� ��ġ�� ���ư���
        OnGround = true;                    // ���� ���� �������� �˸� => ���� �����ϴ�
    }

    // �浹�� ���� ��� == ���߿� �ִ�
    void Character::OnAir()
    {
        OnGround = false;                   // ���߿� ������ �˸���
    }



// ���� �ٶ󺸴� ������ front vec �� ������ ���� �����̰� �ִ� xzDir ����� ��ġ��Ų��
void Character::Rotate()             
{
    // Ȥ�� �𸣴ϱ� ����ȭ �ѹ� ��
    FrontVec.y = 0;
    FrontVec = glm::normalize(FrontVec);

    LeftVec.y = 0;
    LeftVec = glm::normalize(LeftVec);
    
    xzDir.y = 0;
    xzDir = glm::normalize(xzDir);


    /* 
        FrontVec -> xzDir �� ��ġ�ϰ� ������ �Ѵ�
        �� ������ ���� ������ ������ ���ϸ�, �� ���͸� ��ġ��Ű�� ���� ������ �ð�������� �ݽð���������� �� �� �ִ�
        
        FrontVec, xzDir ��� XZ ������ ���� => ������ y �࿡ ������ ���Ͱ� �����Ƿ�
        ���� ������ y �� ������
            ��� ==> �ݽð���� ȸ�� ==> + �������� ȸ��
            ���� ==> �ð���� ȸ�� ==> - �������� ȸ��

            ��? ������ ������ ������ �����ϱ�
     */
    float IsCounterClockWise = (glm::cross(FrontVec, xzDir).y >= 0) ? 1.0f : -1.0f;
    

    /* 
        FrontVec �� xzDir �� �̷�� ���� ���� 0 ~ 180 �� ����
        ������ �̿��ϸ� cos(theta) ���� ���� �� �ִ�
     */
    float CosTheta = glm::dot(FrontVec, xzDir);


    // ( 1 - Cosin Theta ) ���� ���� ���ϸ�, �� ���Ͱ� ����� ����ϴٴ� ��
    // �׳� �� ���͸� ��ġ���� ������
    if( (1 - CosTheta) < YawMinValue )
    {
        FrontVec = xzDir;
        LeftVec = glm::cross(UpVec, FrontVec);    // y, z ���� => x
    }
    // �� ���� ���, ĳ���� �� ��ǥ�踦 ȸ����Ų�� => ���� ȸ���̹Ƿ� ȸ�� ��ȯ�� �״�� �ٷ� �����ص� ������
    else
    {
        auto YawRot = glm::rotate(glm::mat4(1.0f), glm::radians(IsCounterClockWise * YawAngleTick * YawAngleTickRatio), glm::vec3(0.0f, 1.0f, 0.0f));

        FrontVec  = glm::vec3(YawRot * glm::vec4(FrontVec, 0.0f));
        LeftVec   = glm::vec3(YawRot * glm::vec4(LeftVec, 0.0f));
    }
}



// ���� Ű �Է� ��, context �� ���� �ٷ� ȣ��Ǵ� �Լ�
void Character::Jump()
{
    if(!OnGround)        // ���߿� ������ ���� �Ұ���
        return;
    else
        ySpeed += JumpPower * JumpPowerRatio;
}