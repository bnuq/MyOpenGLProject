#ifndef __CONTEXT_H__
#define __CONTEXT_H__

/* 
    ?›?˜ ?™¸ë¶? ?¼?´ë¸ŒëŸ¬ë¦¬ëŠ” common.h ?— ?¬?•¨ ?‹œì¼°ë‹¤
    ?•˜ì§?ë§? imgui ?˜ ê²½ìš°, GL ?„ ?´?š©?•´?„œ ê·¸ë¦¬?Š” ê³¼ì •?—?„œë§? ?‚¬?š©?•˜ë¯?ë¡?
    context ?—ë§? ì¶”ê??ë¥? ?‹œ?‚¨?‹¤
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

    // ?‚¤ë³´ë“œë¥? ?†µ?•´ ?…? ¥?œ ê°’ë“¤?„ ì²˜ë¦¬, ????‘
    void ProcessInput(GLFWwindow* window);

    // ?™”ë©? ?¬ê¸°ê?? ë³??•  ?•Œ, ????‘
    void Reshape(int, int);

    // ë§ˆìš°?Š¤ ?´?™ ë°? ?…? ¥ ì²˜ë¦¬
    void MouseMove(double, double);
    void MouseButton(int button, int action, double x, double y);

    // Space ?ˆŒ????„ ?•Œ ?˜?„ ì£¼ëŠ” ?•¨?ˆ˜
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
    ProgramUPtr m_program;          // phone lighting êµ¬í˜„ ?”„ë¡œê·¸?¨
    


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




    // camera parameter => ?›”?“œ ì¢Œí‘œê³? ê¸°ì?? ì¢Œí‘œ ê°’ë“¤
    glm::vec3 m_cameraPos { glm::vec3(0.0f, 2.5f, 8.0f) };      // ì¹´ë©”?¼?˜ ?œ„ì¹?
        /* 
            ì¹´ë©”?¼ê°? ë°”ë¼ë³´ëŠ” ????ƒ?˜ ì¢Œí‘œ, Target
            ì¹´ë©”?¼ê°? ????ƒ?„ ë°”ë¼ë³´ëŠ” ë°©í–¥ ë²¡í„°, Front
        
            z ì¶? +3 ?— ?œ„?”¼?•´?„œ ?›?  ë°©í–¥?„ ë°”ë¼ë³´ê³  ?ˆ?œ¼ë¯?ë¡?, (0, 0, -1) ë²¡í„°ë¥? camera front ë¡? ê°?ì§„ë‹¤
        */
    glm::vec3 m_cameraFront { glm::vec3(0.0f, 0.0f, -1.0f) };   // ì¹´ë©”?¼ê°? ë°”ë¼ë³´ëŠ” ë°©í–¥ ë²¡í„°
    glm::vec3 m_cameraUp { glm::vec3(0.0f, 1.0f, 0.0f) };       // ì¹´ë©”?¼ UP vector
    // ì¹´ë©”?¼?˜ ?šŒ? „ ê°ë„ => ì¹´ë©”?¼ ì¢Œí‘œê³„ë?? ê¸°ì???œ¼ë¡? ?šŒ? „?•œ?‹¤
    // ì¹´ë©”?¼ê°? ë°”ë¼ë³´ëŠ” ë°©í–¥?´ ì¹´ë©”?¼ ì¢Œí‘œê³„ì˜ Z ì¶?
    float m_cameraPitch { -20.0f };   // ì¹´ë©”?¼ ì¢Œí‘œê³? X ì¶? ê¸°ì?? ?šŒ? „
    float m_cameraYaw { 0.0f };     // ì¹´ë©”?¼ ì¢Œí‘œê³? Y ì¶? ê¸°ì?? ?šŒ? „
    float m_cameraRotSpeed = 0.1f;
    float cameraSpeed = 0.1f;
    bool m_cameraControl { false };

    // Framebuffer
    FramebufferUPtr m_framebuffer;

    // ê´‘ì›
    /* 
        ê´‘ì›?— ????•œ ? •ë³?
            ê´‘ì›?˜ ?œ„ì¹?
            ê´‘ì›?˜ Material

        ?…°?´?”?— struct ë¥? ?™?¼?•œ êµ¬ì¡°ë¡? ?•Œ? ¤ì£¼ë©´
        ?…°?´?”?—?„œ?„ ?•´?‹¹ struct ë¥? ë°›ì•„?„œ ?‚¬?š©?•  ?ˆ˜ ?ˆ?‹¤
     */
    struct Light
    {
        glm::vec3 position { glm::vec3(1.0f, 4.0f, 4.0f) };
        // Spot Light ê°? ë¹„ì¶”?Š” ë°©í–¥
        glm::vec3 direction{ glm::vec3(-1.0f, -1.0f, -1.0f) };
        // Spot Light ê°? ë¹„ì¶”?Š” ê°ë„, ?„ë¡? ?‘œ?˜„
        glm::vec2 cutoff { glm::vec2(120.0f, 5.0f) };
        // omni light
        float distance { 128.0f };

        glm::vec3 ambient { glm::vec3(0.1f, 0.1f, 0.1f) };
        glm::vec3 diffuse { glm::vec3(0.5f, 0.5f, 0.5f) };
        glm::vec3 specular { glm::vec3(1.0f, 1.0f, 1.0f) };
    };
    Light m_light;          // ê´‘ì› ë¨¸í„°ë¦¬ì–¼ ?„ ?–¸
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