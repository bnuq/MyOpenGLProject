#ifndef __CONTEXT_H__
#define __CONTEXT_H__

#include <imgui.h>
#include <vector>
#include <queue>
#include <unordered_map>

#include "common.h"
#include "program.h"
#include "mesh.h"
#include "framebuffer.h"
#include "Player.h"
#include "Camera.h"



// ������ ����� Ŭ����
#include "Box.h"
#include "Character.h"
#include "Floor.h"



CLASS_PTR(Context)
class Context
{
public:
    // �⺻���� �Լ�
    static ContextUPtr Create();
    void Render();

    // Ű����, ���콺 �Է� ó��
    void ProcessInput(GLFWwindow* window);
    void Reshape(int, int);
    void MouseMove(double, double);
    void MouseButton(int button, int action, double x, double y);

    // ���� �÷��̾� ����
    void MakeJump()
    {
        mainChar->Jump();
    }
    void MakeDash()
    {
        mainChar->Dash();
    }



private:
    Context() {}
    bool Init();

    // Window Information
    int m_width { WINDOW_WIDTH };
    int m_height { WINDOW_HEIGHT };
    glm::vec4 m_clearColor { glm::vec4(0.1f, 0.2f, 0.3f, 0.0f) };

    // Mouse
    glm::vec2 m_prevMousePos { glm::vec2(0.0f) };

    // Programs
    ProgramUPtr CharProgram;    // ���� �÷��̾� �׸�
    ProgramUPtr MapProgram;     // �ٴ� �׸�

    // Meshes
    MeshUPtr CharMesh;          // ���� ĳ���͸� �׸��� �޽�
    MeshUPtr FloorMesh;         // �ٴ��� �׸��� �޽�
    
    // Materials    
    MaterialPtr CharMat;
    MaterialPtr FloorMat;
    
    // ī�޶� ��ü
    CameraPtr MainCam;
    glm::vec3 m_cameraPos { glm::vec3(0.0f, 2.5f, 8.0f) }; // �ϴ� light ��꿡 ���
    bool m_cameraControl { false };

    struct Light
    {
        glm::vec3 position { glm::vec3(1.0f, 4.0f, 4.0f) };

        glm::vec3 direction{ glm::vec3(-1.0f, -1.0f, -1.0f) };
        glm::vec2 cutoff { glm::vec2(120.0f, 5.0f) };
        
        glm::vec3 ambient { glm::vec3(0.1f, 0.1f, 0.1f) };
        glm::vec3 diffuse { glm::vec3(0.5f, 0.5f, 0.5f) };
        glm::vec3 specular { glm::vec3(1.0f, 1.0f, 1.0f) };

        float distance { 128.0f };
    };
    Light m_light;


    

    // �÷����� ���� ĳ����
    CharacterPtr mainChar;


    
    // Floor ���� �����ϴ� ����
    std::vector<FloorPtr> GameMap{};




    // gpu instancing �� ���� ��ġ ����
    std::vector<glm::vec3> floorPos{};
    // gpu instancing => �÷��̾�� �ٴ��� ��Ҵ� �� üũ�ϴ� �� ���
    std::vector<glm::vec2> TouchAndDelete{};

    VertexLayoutUPtr floorInstancing;
    BufferUPtr floorPosBuffer;
    BufferUPtr TouchAndDeleteBuffer;




    // Floor Data UBO
    BufferUPtr FloorUBO;


    BufferUPtr FloorSSBO;
    struct FloorStr
    {
        glm::vec3 FloorPos;
        int FloorTouched;
    };
    std::vector<FloorStr> FloorInformation{};
};

#endif // __CONTEXT_H__