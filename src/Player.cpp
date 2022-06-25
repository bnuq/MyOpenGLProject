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
    // 일단 속도를 바꾼다
    Velocity += ((OnGround) ? glm::vec3(0.0f, 0.0f, 0.0f) : Acceleration);
    
    // 이후 위치를 일단 구해
    Position += Velocity;

    if(Position.y > HEIGHT)  // 아직 공중에 떠 있다
    {
        OnGround = false;
    }
    // 땅에 닿았다
    else
    {
        OnGround = true;

        Position.y = HEIGHT;
        Velocity.y = 0;

        // 점프 횟수 초기화
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