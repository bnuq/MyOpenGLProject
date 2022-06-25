#ifndef __CONTEXT_H__
#define __CONTEXT_H__

/* 
    ?��?�� ?���? ?��?��브러리는 common.h ?�� ?��?�� ?��켰다
    ?���?�? imgui ?�� 경우, GL ?�� ?��?��?��?�� 그리?�� 과정?��?���? ?��?��?���?�?
    context ?���? 추�??�? ?��?��?��
 */
#include <imgui.h>

#include "common.h"
#include "program.h"
#include "mesh.h"
#include "framebuffer.h"

CLASS_PTR(Context)
class Context
{
public:
    static ContextUPtr Create();
    void Render();

    // ?��보드�? ?��?�� ?��?��?�� 값들?�� 처리, ????��
    void ProcessInput(GLFWwindow* window);

    // ?���? ?��기�?? �??�� ?��, ????��
    void Reshape(int, int);

    // 마우?�� ?��?�� �? ?��?�� 처리
    void MouseMove(double, double);
    void MouseButton(int button, int action, double x, double y);

    // Space ?��????�� ?�� ?��?�� 주는 ?��?��
    void PlayerJump();
    void PlayerGround();

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
    ProgramUPtr m_program;          // phone lighting 구현 ?��로그?��
    


    struct Camera
    {
        glm::vec3 position { glm::vec3(0.0f, 5.0f, 0.0f) };
        glm::vec3 frontDir { glm::vec3(0.0f, 0.0f, -1.0f) };
        glm::vec3 upDir { glm::vec3(0.0f, 1.0f, 0.0f) };

        float pitch { -20.0f };
        float yaw { 0.0f };

        float moveSpeed { 0.01f };
        float rotSpeed { 0.01f };
    };
    Camera Cam;


    struct Character
    {
        glm::vec3 position { 0.0f, 5.0f, 0.0f };
        glm::vec3 frontDir { glm::vec3(0.0f, 0.0f, -1.0f) };
        glm::vec3 rightDir { glm::vec3(1.0f, 0.0f, 0.0f) };
        
        
        const float acc = -0.01f;
        float velocity = 0.0f;

        bool jumping = false;
        bool onGround = false;

        float moveSpeed { 0.05f };
        float rotSpeed { 0.01f };

        MeshUPtr meshPtr;
        MaterialPtr matPtr;
    };
    Character player;


    MeshUPtr floorPtr;
    MaterialPtr floorMat;




    // camera parameter => ?��?�� 좌표�? 기�?? 좌표 값들
    glm::vec3 m_cameraPos { glm::vec3(0.0f, 2.5f, 8.0f) };      // 카메?��?�� ?���?
        /* 
            카메?���? 바라보는 ????��?�� 좌표, Target
            카메?���? ????��?�� 바라보는 방향 벡터, Front
        
            z �? +3 ?�� ?��?��?��?�� ?��?�� 방향?�� 바라보고 ?��?���?�?, (0, 0, -1) 벡터�? camera front �? �?진다
        */
    glm::vec3 m_cameraFront { glm::vec3(0.0f, 0.0f, -1.0f) };   // 카메?���? 바라보는 방향 벡터
    glm::vec3 m_cameraUp { glm::vec3(0.0f, 1.0f, 0.0f) };       // 카메?�� UP vector
    // 카메?��?�� ?��?�� 각도 => 카메?�� 좌표계�?? 기�???���? ?��?��?��?��
    // 카메?���? 바라보는 방향?�� 카메?�� 좌표계의 Z �?
    float m_cameraPitch { -20.0f };   // 카메?�� 좌표�? X �? 기�?? ?��?��
    float m_cameraYaw { 0.0f };     // 카메?�� 좌표�? Y �? 기�?? ?��?��
    float m_cameraRotSpeed = 0.1f;
    float cameraSpeed = 0.1f;
    bool m_cameraControl { false };

    // Framebuffer
    FramebufferUPtr m_framebuffer;

    // 광원
    /* 
        광원?�� ????�� ?���?
            광원?�� ?���?
            광원?�� Material

        ?��?��?��?�� struct �? ?��?��?�� 구조�? ?��?��주면
        ?��?��?��?��?��?�� ?��?�� struct �? 받아?�� ?��?��?�� ?�� ?��?��
     */
    struct Light
    {
        glm::vec3 position { glm::vec3(1.0f, 4.0f, 4.0f) };
        // Spot Light �? 비추?�� 방향
        glm::vec3 direction{ glm::vec3(-1.0f, -1.0f, -1.0f) };
        // Spot Light �? 비추?�� 각도, ?���? ?��?��
        glm::vec2 cutoff { glm::vec2(120.0f, 5.0f) };
        // omni light
        float distance { 128.0f };

        glm::vec3 ambient { glm::vec3(0.1f, 0.1f, 0.1f) };
        glm::vec3 diffuse { glm::vec3(0.5f, 0.5f, 0.5f) };
        glm::vec3 specular { glm::vec3(1.0f, 1.0f, 1.0f) };
    };
    Light m_light;          // 광원 머터리얼 ?��?��
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