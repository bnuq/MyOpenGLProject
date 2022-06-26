#include <string>

#include "common.h"
#include "mesh.h"
#include "texture.h"
#include "image.h"



CLASS_PTR(Player)
class Player
{
public:
    static PlayerUPtr Create();

    glm::vec3 Position          = glm::vec3(0.0f, 3.0f, 0.0f);
    glm::vec3 Velocity          = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 Acceleration      = glm::vec3(0.0f, -0.02f, 0.0f);

    
    glm::vec3 FrontVec          = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 RightVec          = glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 UpVec             = glm::vec3(0.0f, 1.0f, 0.0f);

    MeshUPtr m_mesh;

    int JumpingCount = 0;
    const int MaxJump = 2;
    bool JumpReleased = true;

    bool OnGround = false;

    float MoveSpeed = 0.1f;
    float RotSpeed;


    void CreateSetMaterial(const std::string& diffusePath, const std::string& specPath, 
                            float shine, bool diffuseFlip = true, bool specFlip = true);

    void MoveXZ(int key);
    void MoveY(float HEIGHT);
    void Jump();


    void Rotate();
    
private:
    Player() { }
};