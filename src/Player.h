#ifndef __PLAYER_H__
#define __PLAYER_H__

#include <string>

#include "../FrameWork/common.h"
#include "../FrameWork/mesh.h"
#include "../FrameWork/texture.h"
#include "../FrameWork/image.h"



CLASS_PTR(Player)
class Player
{
public:
    glm::vec3 Position          = glm::vec3(0.0f, 3.0f, 0.0f);
    glm::vec3 Velocity          = glm::vec3(0.0f, 0.0f, 0.0f);
    
    MeshUPtr m_mesh;

    int JumpingCount = 0;
    const int MaxJump = 2;
    
    bool JumpReleased = true;
    bool OnGround = false;
   
    glm::vec3 FrontVec          = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::vec3 LeftVec          = glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 UpVec             = glm::vec3(0.0f, 1.0f, 0.0f);


/* Setting Values */
    glm::vec3 Acceleration      = glm::vec3(0.0f, -0.02f, 0.0f);
    float JumpPower = 0.5f;
    float MoveSpeed = 0.1f;
    float RotSpeed = 0.1f;



    static PlayerPtr Create();
    void CreateSetMaterial(const std::string& diffusePath, const std::string& specPath, 
                            float shine, bool diffuseFlip = true, bool specFlip = true);

    void MoveXZ(int key);
    void MoveY(float HEIGHT);
    void Jump();
    void Rotate(float);
    
private:
    Player() { }
};

#endif  // __PLAYER_H__