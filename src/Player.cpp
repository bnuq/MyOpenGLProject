#include "Player.h"

PlayerUPtr Player::Create()
{
    auto player = PlayerUPtr(new Player());
    return std::move(player);
}

void Player::MoveXZ(int key)
{
    switch (key)
    {
    case GLFW_KEY_W:
        Position += MoveSpeed * FrontVec;
        break;
    
    case GLFW_KEY_S:
        Position -= MoveSpeed * FrontVec;
        break;

    case GLFW_KEY_D:
        Position += MoveSpeed * RightVec;
        break;

    case GLFW_KEY_A:
        Position -= MoveSpeed * RightVec;
        break;

    default:
        break;
    }
}

void Player::MoveY(float HEIGHT)
{
    // �ϴ� �ӵ��� �ٲ۴�
    Velocity += ((OnGround) ? glm::vec3(0.0f, 0.0f, 0.0f) : Acceleration);
    
    // ���� ��ġ�� �ϴ� ����
    Position += Velocity;

    if(Position.y > HEIGHT)  // ���� ���߿� �� �ִ�
    {
        OnGround = false;
    }
    // ���� ��Ҵ�
    else
    {
        OnGround = true;

        Position.y = HEIGHT;
        Velocity.y = 0;

        // ���� Ƚ�� �ʱ�ȭ
        JumpingCount = 0;
    }
}

void Player::Jump()
{
    if(JumpingCount >= MaxJump) return;

    JumpingCount++;

    
    Velocity += glm::vec3(0.0f, 0.3f, 0.0f);
    SPDLOG_INFO("Velocity {} {} {}", Velocity.x, Velocity.y, Velocity.z);
}