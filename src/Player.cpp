#include "Player.h"

PlayerPtr Player::Create()
{
    auto player = PlayerUPtr(new Player());
    return std::move(player);
}

void Player::CreateSetMaterial(const std::string& diffusePath, const std::string& specPath, float shine, bool diffuseFlip, bool specFlip)
{
    auto TempMat = Material::Create();
    TempMat->diffuse = Texture::CreateFromImage(Image::Load(diffusePath, diffuseFlip).get());
    TempMat->specular = Texture::CreateFromImage(Image::Load(specPath, specFlip).get());
    TempMat->shininess = shine;

    m_mesh->SetMaterial(std::move(TempMat));
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
        Position -= MoveSpeed * LeftVec;
        break;

    case GLFW_KEY_A:
        Position += MoveSpeed * LeftVec;
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

    
    //Velocity += glm::vec3(0.0f, 0.3f, 0.0f);
    Velocity += JumpPower * UpVec;
    SPDLOG_INFO("Velocity {} {} {}", Velocity.x, Velocity.y, Velocity.z);
}


void Player::Rotate(glm::vec2 deltaPos)
{
    float DeltaVal = deltaPos.x * RotSpeed;

    SPDLOG_INFO("DeltaPos x is {}", deltaPos.x);
    SPDLOG_INFO("DeltaVal is {}", DeltaVal);

    
    if (DeltaVal < 0.0f)   DeltaVal += 360.0f;
    if (DeltaVal > 360.0f) DeltaVal -= 360.0f;


    auto YawRot = glm::rotate(glm::mat4(1.0f), glm::radians(DeltaVal), glm::vec3(0.0f, 1.0f, 0.0f));
    FrontVec = glm::vec3(YawRot * glm::vec4(FrontVec, 0.0f));
    LeftVec = glm::vec3(YawRot * glm::vec4(LeftVec, 0.0f));

    
}