#ifndef __CONTEXT_H__
#define __CONTEXT_H__

#include <imgui.h>

#include "common.h"
#include "program.h"
#include "mesh.h"
#include "framebuffer.h"
#include "Player.h"

CLASS_PTR(Context)
class Context
{
public:
    static ContextUPtr Create();
    void Render();

    void ProcessInput(GLFWwindow* window);

    void Reshape(int, int);

    void MouseMove(double, double);
    void MouseButton(int button, int action, double x, double y);

    void MakeJump() { 
        MainBox->Jump();
        MainBox->JumpReleased = false;
    }

private:
    Context() {}
    bool Init();

    // Window
    int m_width { WINDOW_WIDTH };
    int m_height { WINDOW_HEIGHT };
    glm::vec4 m_clearColor { glm::vec4(0.1f, 0.2f, 0.3f, 0.0f) };

    // Mouse
    glm::vec2 m_prevMousePos { glm::vec2(0.0f) };

    // Programs
    ProgramUPtr m_program;          // phone lighting κ΅¬ν ?λ‘κ·Έ?¨
    



    PlayerUPtr MainBox;

    MeshUPtr floorPtr;
    MaterialPtr floorMat;




    // camera parameter => ?? μ’νκ³? κΈ°μ?? μ’ν κ°λ€
    glm::vec3 m_cameraPos { glm::vec3(0.0f, 2.5f, 8.0f) };      // μΉ΄λ©?Ό? ?μΉ?
        /* 
            μΉ΄λ©?Όκ°? λ°λΌλ³΄λ ????? μ’ν, Target
            μΉ΄λ©?Όκ°? ????? λ°λΌλ³΄λ λ°©ν₯ λ²‘ν°, Front
        
            z μΆ? +3 ? ??Ό?΄? ??  λ°©ν₯? λ°λΌλ³΄κ³  ??Όλ―?λ‘?, (0, 0, -1) λ²‘ν°λ₯? camera front λ‘? κ°?μ§λ€
        */
    glm::vec3 m_cameraFront { glm::vec3(0.0f, 0.0f, -1.0f) };   // μΉ΄λ©?Όκ°? λ°λΌλ³΄λ λ°©ν₯ λ²‘ν°
    glm::vec3 m_cameraUp { glm::vec3(0.0f, 1.0f, 0.0f) };       // μΉ΄λ©?Ό UP vector
    // μΉ΄λ©?Ό? ??  κ°λ => μΉ΄λ©?Ό μ’νκ³λ?? κΈ°μ???Όλ‘? ?? ??€
    // μΉ΄λ©?Όκ°? λ°λΌλ³΄λ λ°©ν₯?΄ μΉ΄λ©?Ό μ’νκ³μ Z μΆ?
    float m_cameraPitch { -20.0f };   // μΉ΄λ©?Ό μ’νκ³? X μΆ? κΈ°μ?? ?? 
    float m_cameraYaw { 0.0f };     // μΉ΄λ©?Ό μ’νκ³? Y μΆ? κΈ°μ?? ?? 
    float m_cameraRotSpeed = 0.1f;
    float cameraSpeed = 0.1f;
    bool m_cameraControl { false };

    // Framebuffer
    FramebufferUPtr m_framebuffer;

    // κ΄μ
    /* 
        κ΄μ? ???? ? λ³?
            κ΄μ? ?μΉ?
            κ΄μ? Material

        ?°?΄?? struct λ₯? ??Ό? κ΅¬μ‘°λ‘? ?? €μ£Όλ©΄
        ?°?΄???? ?΄?Ή struct λ₯? λ°μ? ?¬?©?  ? ??€
     */
    struct Light
    {
        glm::vec3 position { glm::vec3(1.0f, 4.0f, 4.0f) };
        // Spot Light κ°? λΉμΆ? λ°©ν₯
        glm::vec3 direction{ glm::vec3(-1.0f, -1.0f, -1.0f) };
        // Spot Light κ°? λΉμΆ? κ°λ, ?λ‘? ??
        glm::vec2 cutoff { glm::vec2(120.0f, 5.0f) };
        // omni light
        float distance { 128.0f };

        glm::vec3 ambient { glm::vec3(0.1f, 0.1f, 0.1f) };
        glm::vec3 diffuse { glm::vec3(0.5f, 0.5f, 0.5f) };
        glm::vec3 specular { glm::vec3(1.0f, 1.0f, 1.0f) };
    };
    Light m_light;          // κ΄μ λ¨Έν°λ¦¬μΌ ? ?Έ
    bool m_flashLightMode { false };

    // Meshes
    MeshUPtr m_box;
    MeshUPtr m_plane;
    
    // Materials    
    MaterialPtr m_planeMaterial;
    MaterialPtr m_box1Material;
    MaterialPtr m_box2Material;

    TexturePtr m_windowTexture;

    // cubemap
    CubeTextureUPtr m_cubeTexture;

};

#endif // __CONTEXT_H__