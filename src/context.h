#ifndef __CONTEXT_H__
#define __CONTEXT_H__

/* 
    원래 외부 라이브러리는 common.h 에 포함 시켰다
    하지만 imgui 의 경우, GL 을 이용해서 그리는 과정에서만 사용하므로
    context 에만 추가를 시킨다
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

    // 키보드를 통해 입력된 값들을 처리, 대응
    void ProcessInput(GLFWwindow* window);

    // 화면 크기가 변할 때, 대응
    void Reshape(int, int);

    // 마우스 이동 및 입력 처리
    void MouseMove(double, double);
    void MouseButton(int button, int action, double x, double y);

    // Space 눌렀을 때 힘을 주는 함수
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
    ProgramUPtr m_program;          // phone lighting 구현 프로그램
    


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




    // camera parameter => 월드 좌표계 기준 좌표 값들
    glm::vec3 m_cameraPos { glm::vec3(0.0f, 2.5f, 8.0f) };      // 카메라의 위치
        /* 
            카메라가 바라보는 대상의 좌표, Target
            카메라가 대상을 바라보는 방향 벡터, Front
        
            z 축 +3 에 위피해서 원점 방향을 바라보고 있으므로, (0, 0, -1) 벡터를 camera front 로 가진다
        */
    glm::vec3 m_cameraFront { glm::vec3(0.0f, 0.0f, -1.0f) };   // 카메라가 바라보는 방향 벡터
    glm::vec3 m_cameraUp { glm::vec3(0.0f, 1.0f, 0.0f) };       // 카메라 UP vector
    // 카메라의 회전 각도 => 카메라 좌표계를 기준으로 회전한다
    // 카메라가 바라보는 방향이 카메라 좌표계의 Z 축
    float m_cameraPitch { -20.0f };   // 카메라 좌표계 X 축 기준 회전
    float m_cameraYaw { 0.0f };     // 카메라 좌표계 Y 축 기준 회전
    float m_cameraRotSpeed = 0.1f;
    float cameraSpeed = 0.1f;
    bool m_cameraControl { false };

    // Framebuffer
    FramebufferUPtr m_framebuffer;

    // 광원
    /* 
        광원에 대한 정보
            광원의 위치
            광원의 Material

        셰이더에 struct 를 동일한 구조로 알려주면
        셰이더에서도 해당 struct 를 받아서 사용할 수 있다
     */
    struct Light
    {
        glm::vec3 position { glm::vec3(1.0f, 4.0f, 4.0f) };
        // Spot Light 가 비추는 방향
        glm::vec3 direction{ glm::vec3(-1.0f, -1.0f, -1.0f) };
        // Spot Light 가 비추는 각도, 도로 표현
        glm::vec2 cutoff { glm::vec2(120.0f, 5.0f) };
        // omni light
        float distance { 128.0f };

        glm::vec3 ambient { glm::vec3(0.1f, 0.1f, 0.1f) };
        glm::vec3 diffuse { glm::vec3(0.5f, 0.5f, 0.5f) };
        glm::vec3 specular { glm::vec3(1.0f, 1.0f, 1.0f) };
    };
    Light m_light;          // 광원 머터리얼 선언
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