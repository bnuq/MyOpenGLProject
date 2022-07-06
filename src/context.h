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
        int     COUNT    = 4;
        float   STRIDE   = 10;
        int     STORY    = 3;
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
         */
        glm::vec4 position;
        /* 
            x   = collision
            y   = disappear
            z   = save time
            w   = limit time
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




/* Initializing */
    void InitGameMap();
    void CreateSSBO();


    void CreateComputeProgram();
    void SetComputeUniformOnce();
    void ConnectShaderAndSSBO();



    void UpdateTiles();


};

#endif // __CONTEXT_H__