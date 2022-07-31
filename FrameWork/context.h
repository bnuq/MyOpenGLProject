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

    // 키보드, 마우스 입력 처리
    void ProcessInput(GLFWwindow* window);
    void Reshape(int, int);
    void MouseMove(double, double);
    void MouseButton(int button, int action, double x, double y);


    // 메인 플레이어 동작
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
    ProgramUPtr CharProgram;            // 메인 플레이어 Draw
    ProgramUPtr MapProgram;             // 바닥 Draw
    ProgramUPtr ComputeProgram;         // 타일의 상태를 계산하는 프로그램
    int ComputeGroupNum;                // compute program 을 실행시킬 그룹의 개수
    ProgramUPtr AlphaMapProgram;        // 반투명한 타일들을 그리는 프로그램


    // Meshes
    MeshUPtr CharMesh;                  // 메인 캐릭터 Mesh
    MeshUPtr FloorMesh;                 // 바닥 Mesh
    

    // Materials
    MaterialPtr CharMat;
    MaterialPtr FloorMat;
    

    // 카메라 객체
    CameraPtr MainCam;
    glm::vec3 m_cameraPos { glm::vec3(0.0f, 2.5f, 8.0f) };
    bool m_cameraControl { false };


    // 광원
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


    // 플레이할 메인 캐릭터
    CharacterPtr mainChar;


    // 전체 Game Map 에 대한 정보
    struct Map
    {
        int     COUNT    = 5;
        float   STRIDE   = 10;
        int     STORY    = 4;
    };
    const Map gameMap{};
    

    /* 
        바닥을 이루는 타일 데이터
        state   = 0.0f = 초기 상태
                = 1.0f = 충돌 상태
                = 2.0f = 사라진 상태
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
        CPU 와 GPU 사이에서 주고받는 데이터
        float
            어떤 작업의 처리 유무를 알리는 역할
            -1 or +1 으로 토글
        unsigned int
            처리된 타일의 번호
     */
    struct OutputData
    {
        float HasCharCollision;             // 메인 캐릭터와 타일 사이의 충돌여부 확인

        float HasWriteColTile;              // GPU -> CPU, 계산결과 충돌한 타일이 있음을 알림
        float HasInformColTile;             // CPU -> GPU, 충돌 처리한 타일이 있음을 알림
        float HasInformDisTile;             // CPU -> GPU, 사라짐 처리한 타일이 있음을 알림
        
        unsigned int WriteColTile;          // GPU -> CPU, 충돌한 타일 번호를 넘김
        unsigned int InformColTile;         // CPU -> GPU, 충돌 처리한 타일 번호
        unsigned int InformDisTile;         // CPU -> GPU, 사라짐 처리한 타일 번호
    };
    OutputData outputdata = OutputData
                            {
                                -1.0f,                      // 처음 메인 캐릭터는 타일과 충돌하지 않은 상태이며
                                -1.0f, -1.0f, -1.0f,        // 충돌하거나 사라진 타일이 전혀 없다
                                0, 0, 0
                            };

    
    // SSBO 버퍼
    BufferPtr tileBuffer;                   // 전체 타일들의 실시간 정보를 저장
    BufferPtr outputBuffer;                 // output data 를 저장한 버퍼, CPU 와 GPU 에서 값을 읽는다


    // SSBO 연결 인덱스
    int tile_binding = 1;
    int output_binding = 2;




    /*  Initializing  */
    void InitGameMap();                     // 전체 Game Map 을 이루는 타일 데이터를 초기화
    
 

    void SetComputeUniformOnce();
    void SetMapUniformOnce();
    void SetAlphaMapUniformOnce();


    void UpdateTiles();




    /* 
        충돌상태 => 사라지고 있는 타일의 정보를 저장하는 map
        카메라->위치 사이의 거리를 키로 가지는 map
            근데 카메라 위치는 매 프레임 바뀔 수 있잖아
            그럼 매번 바뀐 카메라 위치 -> collision 상태인 타일들 사이의 거리를 계산해서
            다시 map 에 넣은 다음에
            Draw Call 하기 전에 다시 넣어주어야 한다?
        그럼 위치 정보 랑 색깔 정보만 있으면 되나?
        위치 정보는 유니폼 변수로 넣어주고
        색깔 정보가 들어가기 보단, 그냥 층수만 알면 되잖아
        그럼 value = glm::vec4 => position + story
        이렇게 구성하면 될듯??


        키 = 타일의 인덱스
            근데 정렬은 카메라와 타일 사이 거리로 하고 싶다
        값 = 타일에 대한 정보
     */

    std::map<float, glm::vec4, std::greater<float>> OrderedCollTiles{};

    /* 
        그럼 충돌한 타일들을 일단 저장해놓는 자료구조가 필요하다
        여기에 일단 저장해 둔 다음, iterate 하면서 map 을 채운다
        
        그리고, disappear 하는 게 있는 지 찾을 수 도 있어야 한다
        iteration + search 둘 다 가능해야 한다
        저장되는 순서가 중요한 건 아니니까, hash function 을 이용하자
        똑같이 위치랑 층수를 저장
     */
    std::unordered_map<unsigned int, glm::vec4> CollIndex{};



    std::deque<std::pair<unsigned int, double>> IndexQueue{};



    double LimitTime = 5.0;








    ShaderPtr testShader;
    ProgramUPtr testProgram;
    BufferPtr testBuffer;
};

#endif // __CONTEXT_H__