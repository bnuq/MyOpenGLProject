#ifndef __CONTEXT_H__
#define __CONTEXT_H__

#include <imgui.h>
#include <vector>
#include <map>
#include <deque>

// FrameWork
#include "common.h"
#include "program.h"
#include "mesh.h"
#include "shadow_map.h"

// My Code
#include "../Src/Camera.h"
#include "../Src/Character.h"
#include "../Src/Light.h"



CLASS_PTR(Context)
class Context
{
public:

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


private:
    Context() {}
    bool Init();


    // Window Information
    int m_width { WINDOW_WIDTH };
    int m_height { WINDOW_HEIGHT };
    glm::vec4 m_clearColor { glm::vec4(0.1f, 0.2f, 0.3f, 0.0f) };


    // Mouse
    glm::vec2 m_prevMousePos { glm::vec2(0.0f) };
    bool Update_Tiles = false;          // Ÿ�ϵ��� ���� ������Ʈ �Լ� �۵��� �����ϴ� ����ġ


    // Shaders
    ShaderPtr ComputeShader;


    // Programs
    ProgramUPtr CharProgram;            // ���� �÷��̾� Draw
    ProgramUPtr MapProgram;             // �ٴ� Draw
    ProgramUPtr ComputeProgram;         // Ÿ���� ���¸� ����ϴ� ���α׷�
    int ComputeGroupNum;                // compute program �� �����ų �׷��� ����
    ProgramUPtr AlphaMapProgram;        // �������� Ÿ�ϵ��� �׸��� ���α׷�

    ProgramUPtr SimpleProgram;          // ������ ���� �ʰ�, �׳� ������ ������������ ��ġ�� ���α׷�


    // Meshes
    MeshUPtr CharMesh;                  // ���� ĳ���� Mesh
    MeshUPtr TileMesh;                  // �ٴ��� �����ϴ� Tile Mesh
    

    // Materials
    MaterialPtr CharMat;
    

    // ī�޶� ��ü
    CameraPtr MainCam;
    bool m_cameraControl { false };


    // Frame Buffer
    ShadowMapUPtr shadow_map_buffer{};         // shadow map �� ����� frame buffer


    // ����
    LightUPtr MainLight;


    // �÷����� ���� ĳ����
    CharacterPtr mainChar;


    // ��ü Game Map �� ���� ����
    struct Map
    {
        unsigned int     COUNT    = 10;
        float            STRIDE   = 10.0f;
        unsigned int     STORY    = 7;
    };
    const Map gameMap{};
    

    /* 
        �ٴ��� �̷�� Ÿ�� ������
        state   = 0 = �ʱ� ����
                = 1 = �浹 ����
                = 2 = ����� ����
     */
    struct Tile
    {
        float xpos;
        float ypos;
        float zpos;
        unsigned int state;
        unsigned int story;
    };
    std::vector<Tile> tileArr{};


    /* 
        CPU �� GPU ���̿��� �ְ�޴� ������
        float
            � �۾��� ó�� ������ �˸��� ����
            -1 or +1 ���� ���
        unsigned int
            ó���� Ÿ���� ��ȣ
     */
    struct OutputData
    {
        float HasCharCollision;             // ���� ĳ���Ϳ� Ÿ�� ������ �浹���� Ȯ��

        float HasWriteColTile;              // GPU -> CPU, ����� �浹�� Ÿ���� ������ �˸�
        float HasInformColTile;             // CPU -> GPU, �浹 ó���� Ÿ���� ������ �˸�
        float HasInformDisTile;             // CPU -> GPU, ����� ó���� Ÿ���� ������ �˸�
        
        unsigned int WriteColTile;          // GPU -> CPU, �浹�� Ÿ�� ��ȣ�� �ѱ�
        unsigned int InformColTile;         // CPU -> GPU, �浹 ó���� Ÿ�� ��ȣ
        unsigned int InformDisTile;         // CPU -> GPU, ����� ó���� Ÿ�� ��ȣ
    };
    OutputData outputdata = OutputData
                            {
                                -1.0f,                      // ó�� ���� ĳ���ʹ� Ÿ�ϰ� �浹���� ���� �����̸�
                                -1.0f, -1.0f, -1.0f,        // �浹�ϰų� ����� Ÿ���� ���� ����
                                0, 0, 0
                            };

    
    // SSBO ����
    BufferPtr tileBuffer;                   // ��ü Ÿ�ϵ��� �ǽð� ������ ����
    BufferPtr outputBuffer;                 // output data �� ������ ����, CPU �� GPU ���� ���� �д´�


    // SSBO ���� �ε���
    int tile_binding = 1;
    int output_binding = 2;



    /* Initialize */
    void InitGameMap();                     // ��ü Game Map �� �̷�� Ÿ�� �����͸� �ʱ�ȭ
    
    void SetComputeUniformOnce();           // �� ���α׷����� �ѹ��� �ʱ�ȭ�ϸ� �Ǵ� Uniform Variables �� �ʱ�ȭ
    void SetMapUniformOnce();
    void SetAlphaMapUniformOnce();


    /* Update */
    void UpdateTiles();                     // �� �����Ӹ���, Ÿ���� ���¸� ����ϰ� ������Ʈ



    /* 
        (�浹�� Ÿ�ϵ��� �ε���, �浹 �ð�) �� ������� �����ϴ� ť
        ���� �� Limit Time �� ������ ť���� �������

        �ش� ť�� ���� Ÿ�ϵ��� �浹�ϰ� ����, ���� ������� ���� �����̹Ƿ�
        �������ϰ� �������Ѵ�
     */
    std::deque<std::pair<unsigned int, double>> IndexQueue{};
    double LimitTime = 5.0;

};

#endif // __CONTEXT_H__