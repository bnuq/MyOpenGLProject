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


private:
    Context() {}
    bool Init();


    // Window Information
    int m_width { WINDOW_WIDTH };
    int m_height { WINDOW_HEIGHT };
    glm::vec4 m_clearColor { glm::vec4(0.1f, 0.2f, 0.3f, 0.0f) };


    // Mouse
    glm::vec2 m_prevMousePos { glm::vec2(0.0f) };
    bool Update_Tiles = false;          // 타일들의 상태 업데이트 함수 작동을 관리하는 스위치


    // Shaders
    ShaderPtr ComputeShader;


    // Programs
    ProgramUPtr CharProgram;            // 메인 플레이어 Draw
    ProgramUPtr MapProgram;             // 바닥 Draw
    ProgramUPtr ComputeProgram;         // 타일의 상태를 계산하는 프로그램
    int ComputeGroupNum;                // compute program 을 실행시킬 그룹의 개수
    ProgramUPtr AlphaMapProgram;        // 반투명한 타일들을 그리는 프로그램

    ProgramUPtr SimpleProgram;          // 색깔을 내지 않고, 그냥 렌더링 파이프라인을 거치는 프로그램


    // Meshes
    MeshUPtr CharMesh;                  // 메인 캐릭터 Mesh
    MeshUPtr TileMesh;                  // 바닥을 구성하는 Tile Mesh
    

    // Materials
    MaterialPtr CharMat;
    

    // 카메라 객체
    CameraPtr MainCam;
    bool m_cameraControl { false };


    // Frame Buffer
    ShadowMapUPtr shadow_map_buffer{};         // shadow map 을 만드는 frame buffer


    // 광원
    LightUPtr MainLight;


    // 플레이할 메인 캐릭터
    CharacterPtr mainChar;


    // 전체 Game Map 에 대한 정보
    struct Map
    {
        unsigned int     COUNT    = 10;
        float            STRIDE   = 10.0f;
        unsigned int     STORY    = 7;
    };
    const Map gameMap{};
    

    /* 
        바닥을 이루는 타일 데이터
        state   = 0 = 초기 상태
                = 1 = 충돌 상태
                = 2 = 사라진 상태
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



    /* Initialize */
    void InitGameMap();                     // 전체 Game Map 을 이루는 타일 데이터를 초기화
    
    void SetComputeUniformOnce();           // 각 프로그램에서 한번만 초기화하면 되는 Uniform Variables 를 초기화
    void SetMapUniformOnce();
    void SetAlphaMapUniformOnce();


    /* Update */
    void UpdateTiles();                     // 매 프레임마다, 타일의 상태를 계산하고 업데이트



    /* 
        (충돌한 타일들의 인덱스, 충돌 시간) 을 순서대로 저장하는 큐
        들어온 후 Limit Time 이 지나면 큐에서 사라진다

        해당 큐에 들어온 타일들은 충돌하고 나서, 아직 사라지지 않은 상태이므로
        반투명하게 렌더링한다
     */
    std::deque<std::pair<unsigned int, double>> IndexQueue{};
    double LimitTime = 5.0;

};

#endif // __CONTEXT_H__