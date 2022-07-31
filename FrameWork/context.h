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
#include "framebuffer.h"

// My Code
#include "../Src/Camera.h"
#include "../Src/Character.h"


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


    // Shaders
    ShaderPtr ComputeShader;


    // Programs
    ProgramUPtr CharProgram;            // ���� �÷��̾� Draw
    ProgramUPtr MapProgram;             // �ٴ� Draw
    ProgramUPtr ComputeProgram;         // Ÿ���� ���¸� ����ϴ� ���α׷�
    int ComputeGroupNum;                // compute program �� �����ų �׷��� ����
    ProgramUPtr AlphaMapProgram;        // �������� Ÿ�ϵ��� �׸��� ���α׷�


    // Meshes
    MeshUPtr CharMesh;                  // ���� ĳ���� Mesh
    MeshUPtr FloorMesh;                 // �ٴ� Mesh
    

    // Materials
    MaterialPtr CharMat;
    MaterialPtr FloorMat;
    

    // ī�޶� ��ü
    CameraPtr MainCam;
    glm::vec3 m_cameraPos { glm::vec3(0.0f, 2.5f, 8.0f) };
    bool m_cameraControl { false };


    // ����
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


    // ��ü Game Map �� ���� ����
    struct Map
    {
        int     COUNT    = 5;
        float   STRIDE   = 10;
        int     STORY    = 4;
    };
    const Map gameMap{};
    

    /* 
        �ٴ��� �̷�� Ÿ�� ������
        state   = 0.0f = �ʱ� ����
                = 1.0f = �浹 ����
                = 2.0f = ����� ����
     */
    struct Tile
    {
        float xpos;
        float ypos;
        float zpos;
        float state;
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




    /*  Initializing  */
    void InitGameMap();                     // ��ü Game Map �� �̷�� Ÿ�� �����͸� �ʱ�ȭ
    
 

    void SetComputeUniformOnce();
    void SetMapUniformOnce();
    void SetAlphaMapUniformOnce();


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



    std::deque<std::pair<unsigned int, double>> IndexQueue{};



    double LimitTime = 5.0;








    ShaderPtr testShader;
    ProgramUPtr testProgram;
    BufferPtr testBuffer;
};

#endif // __CONTEXT_H__