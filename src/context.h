#ifndef __CONTEXT_H__
#define __CONTEXT_H__

#include <imgui.h>
#include <vector>
#include <queue>

#include "common.h"
#include "program.h"
#include "mesh.h"
#include "framebuffer.h"
#include "Player.h"
#include "Camera.h"
#include "Floor.h"


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
    ProgramUPtr m_program;
    ProgramUPtr MapProgram;
    

    PlayerPtr MainBox;
    CameraPtr MainCam;


    MeshPtr floorPtr;
    MaterialPtr floorMat;


    std::vector<Floor> FloorArr {};
    std::queue<float> FloorHeight{};


    
    glm::vec3 m_cameraPos { glm::vec3(0.0f, 2.5f, 8.0f) };
    bool m_cameraControl { false };

    // Framebuffer
    FramebufferUPtr m_framebuffer;

    
    struct Light
    {
        glm::vec3 position { glm::vec3(1.0f, 4.0f, 4.0f) };
        // Spot Light κ°? λΉ„μ¶”?” λ°©ν–¥
        glm::vec3 direction{ glm::vec3(-1.0f, -1.0f, -1.0f) };
        // Spot Light κ°? λΉ„μ¶”?” κ°λ„, ?„λ΅? ?‘?„
        glm::vec2 cutoff { glm::vec2(120.0f, 5.0f) };
        // omni light
        float distance { 128.0f };

        glm::vec3 ambient { glm::vec3(0.1f, 0.1f, 0.1f) };
        glm::vec3 diffuse { glm::vec3(0.5f, 0.5f, 0.5f) };
        glm::vec3 specular { glm::vec3(1.0f, 1.0f, 1.0f) };
    };
    Light m_light;          // κ΄‘μ› λ¨Έν„°λ¦¬μ–Ό ?„ ?–Έ
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