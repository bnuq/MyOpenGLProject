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



// 앞으로 사용할 클래스
#include "Box.h"
#include "Character.h"
#include "Floor.h"



CLASS_PTR(Context)
class Context
{
public:
    // 기본적인 함수
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

    // Programs
    ProgramUPtr CharProgram;    // 메인 플레이어 그림
    ProgramUPtr MapProgram;     // 바닥 그림

    // Meshes
    MeshUPtr CharMesh;          // 메인 캐릭터를 그리는 메쉬
    MeshUPtr FloorMesh;         // 바닥을 그리는 메쉬
    
    // Materials
    MaterialPtr CharMat;
    MaterialPtr FloorMat;
    
    // 카메라 객체
    CameraPtr MainCam;
    glm::vec3 m_cameraPos { glm::vec3(0.0f, 2.5f, 8.0f) }; // 일단 light 계산에 사용
    bool m_cameraControl { false };

    // 빛, 라이트
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

    // Floor 들이 모여서 만드는 전체 Map
    std::vector<std::vector<FloorPtr>> GameMap{};


    // Map 을 구성하는 여러 요소
    struct Map
    {
        int     COUNT    = 2;
        float   STRIDE   = 10;
        int     STORY    = 2;
    };
    const Map gameMap{};
    


    
    // SSBO 버퍼를 관리하는 객체
    BufferPtr tileBuffer;
    int tile_binding = 1;

    BufferPtr outputBuffer;
    int output_binding = 2;


    glm::vec4 CharHit = glm::vec4(0, 0, 0, 0);

    // Floor 대신, 타일을 구성하는 구조
    struct Tile
    {
        /* 
            xyz = Position
            w   = story, 위치한 층 수를 저장
                  위 -> 밑 가면서, 1, 2, 3 ... 순서
         */
        glm::vec4 position;
        /* 
            x   = collision
            y   = disappear
            z   = save time
            w   = collision 이 체크됐는 지 확인
         */
        glm::vec4 collAndTime;
    };
    // 타일 데이터의 배열
    std::vector<Tile> tileArr{};

    // Compute Program
    ProgramUPtr ComputeProgram;
    // compute shader
    ShaderPtr ComputeShader;
    // compute shader 를 실행시킬 그룹의 개수
    int ComputeGroupNum;



    struct OutputData
    {
        /* 
            x = 캐릭터 충돌유무
            y = 갱신 collision index
            z = 갱신 disappear index
            w = collision 갱신 여부, 0 이면 갱신되지 않았다는 것
        */
        glm::vec4 collCheck;

        /* 
            갱신 collision 타일의 position + height
         */
        glm::vec4 collData;
    };
    OutputData outputdata = OutputData{
        // w = 0.0f => collision 갱신된 타일이 없다
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


};

#endif // __CONTEXT_H__