#ifndef __CONTEXT_H__
#define __CONTEXT_H__

#include <imgui.h>
#include <vector>
#include <map>


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

    // ��, ����Ʈ
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

    // Floor ���� �𿩼� ����� ��ü Map
    std::vector<std::vector<FloorPtr>> GameMap{};


    // Map �� �����ϴ� ���� ���
    struct Map
    {
        int     COUNT    = 2;
        float   STRIDE   = 10;
        int     STORY    = 2;
    };
    const Map gameMap{};
    


    
    // SSBO ���۸� �����ϴ� ��ü
    BufferPtr tileBuffer;
    int tile_binding = 1;

    BufferPtr outputBuffer;
    int output_binding = 2;


    glm::vec4 CharHit = glm::vec4(0, 0, 0, 0);

    // Floor ���, Ÿ���� �����ϴ� ����
    struct Tile
    {
        /* 
            xyz = Position
            w   = story, ��ġ�� �� ���� ����
                  �� -> �� ���鼭, 1, 2, 3 ... ����
         */
        glm::vec4 position;
        /* 
            x   = collision
            y   = disappear
            z   = save time
            w   = collision �� üũ�ƴ� �� Ȯ��
         */
        glm::vec4 collAndTime;
    };
    // Ÿ�� �������� �迭
    std::vector<Tile> tileArr{};

    // Compute Program
    ProgramUPtr ComputeProgram;
    // compute shader
    ShaderPtr ComputeShader;
    // compute shader �� �����ų �׷��� ����
    int ComputeGroupNum;



    struct OutputData
    {
        /* 
            x = ĳ���� �浹����
            y = ���� collision index
            z = ���� disappear index
            w = collision ���� ����, 0 �̸� ���ŵ��� �ʾҴٴ� ��
        */
        glm::vec4 collCheck;

        /* 
            ���� collision Ÿ���� position + height
         */
        glm::vec4 collData;
    };
    OutputData outputdata = OutputData{
        // w = 0.0f => collision ���ŵ� Ÿ���� ����
        glm::vec4(0.0f, -1.0f, -1.0f, 0.0f),
        glm::vec4(0.0f, 0.0f, 0.0f, 0.0f)
    };




/* Initializing */
    void InitGameMap();
    void CreateSSBO();


    void CreateComputeProgram();
    
    void SetComputeUniformOnce();

    void SetMapUniformOnce();



    void ConnectShaderAndSSBO();



    void UpdateTiles();




    /* 
        �浹���� => ������� �ִ� Ÿ���� ������ �����ϴ� map
        ī�޶�->��ġ ������ �Ÿ��� Ű�� ������ map
            �ٵ� ī�޶� ��ġ�� �� ������ �ٲ� �� ���ݾ�
            �׷� �Ź� �ٲ� ī�޶� ��ġ -> collision ������ Ÿ�ϵ� ������ �Ÿ��� ����ؼ�
            �ٽ� map �� ���� ������
            Draw Call �ϱ� ���� �ٽ� �־��־�� �Ѵ�?
        �׷� ��ġ ���� �� ���� ������ ������ �ǳ�?
        ��ġ ������ ������ ������ �־��ְ�
        ���� ������ ���� ����, �׳� ������ �˸� ���ݾ�
        �׷� value = glm::vec4 => position + story
        �̷��� �����ϸ� �ɵ�??


        Ű = Ÿ���� �ε���
            �ٵ� ������ ī�޶�� Ÿ�� ���� �Ÿ��� �ϰ� �ʹ�
        �� = Ÿ�Ͽ� ���� ����
     */

    std::map<float, glm::vec4, std::greater<float>> OrderedCollTiles{};

    /* 
        �׷� �浹�� Ÿ�ϵ��� �ϴ� �����س��� �ڷᱸ���� �ʿ��ϴ�
        ���⿡ �ϴ� ������ �� ����, iterate �ϸ鼭 map �� ä���
        
        �׸���, disappear �ϴ� �� �ִ� �� ã�� �� �� �־�� �Ѵ�
        iteration + search �� �� �����ؾ� �Ѵ�
        ����Ǵ� ������ �߿��� �� �ƴϴϱ�, hash function �� �̿�����
        �Ȱ��� ��ġ�� ������ ����
     */
    std::unordered_map<unsigned int, glm::vec4> CollIndex{};


};

#endif // __CONTEXT_H__