#include "context.h"
#include <vector>
#include <string>


ContextUPtr Context::Create()                                  
{
    auto context = ContextUPtr(new Context());
    if (!context->Init())
        return nullptr;
    return std::move(context);
}


bool Context::Init()
{
    // Clear Color
    glClearColor(0.1f, 0.2f, 0.3f, 0.0f);


    // Main Character 그리는 Program 생성
        CharProgram = Program::Create("./shader/Character/character.vs", "./shader/Character/character.fs");
        if(!CharProgram) return false;


    // Map 그리는 Program 생성 <= GPU Instancing
        MapProgram = Program::Create("./shader/Map/map.vs", "./shader/Map/map.fs");
        if(!MapProgram) return false;


    // Compute Program 생성
        // compute shader 를 직접 먼저 생성한 다음, 붙인다
        ComputeShader = Shader::CreateFromFile("./shader/Compute/TileCheck.compute", GL_COMPUTE_SHADER);
        ComputeProgram = Program::Create({ComputeShader});
        if(!ComputeProgram) return false;


    // 반투명한 Map 을 그리는 Program 생성
        AlphaMapProgram = Program::Create("./shader/Map/alphamap.vs", "./shader/Map/alphamap.fs");
        if(!AlphaMapProgram) return false;




    // 전체 게임 맵을 생성한다 ==> CPU 내의 데이터 초기화
        InitGameMap();
    

    // CPU 데이터를 GPU 내에서 저장하는 Shader Storage Buffer Object 를 생성
        // 타일 데이터 => 변하는 타일들의 데이터가 그대로 저장됨
        tileBuffer 
            = Buffer::CreateWithData(GL_SHADER_STORAGE_BUFFER, GL_DYNAMIC_DRAW, tileArr.data(), sizeof(Tile), tileArr.size());
        
        // output 데이터 => CPU 와 GPU 끼리 주고받는 데이터
        outputBuffer 
            = Buffer::CreateWithData(GL_SHADER_STORAGE_BUFFER, GL_DYNAMIC_DRAW, &(outputdata), sizeof(OutputData), 1);


    // 각 SSBO 버퍼를 Binding Point 에 연결한다
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, tile_binding,   tileBuffer->Get());
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, output_binding, outputBuffer->Get());


    // SSBO 를 사용할 프로그램을 Binding Point 에 연결한다
        // Compute Program => 타일들의 상태를 계산하고, output data 를 통해 결과를 주고 받는다
        auto block_index = glGetProgramResourceIndex(ComputeProgram->Get(), GL_SHADER_STORAGE_BUFFER, "TileBuffer");
        glShaderStorageBlockBinding(ComputeProgram->Get(), block_index, tile_binding);

        block_index = glGetProgramResourceIndex(ComputeProgram->Get(), GL_SHADER_STORAGE_BUFFER, "OutputBuffer");
        glShaderStorageBlockBinding(ComputeProgram->Get(), block_index, output_binding);

        // Map Program => 타일 렌더링, 타일 데이터만 필요
        block_index = glGetProgramResourceIndex(MapProgram->Get(), GL_SHADER_STORAGE_BUFFER, "TileBuffer");
        glShaderStorageBlockBinding(MapProgram->Get(), block_index, tile_binding);

    


    // 메인 캐릭터
    mainChar = CharacterPtr(new Character(glm::vec3(0.0f, 10.0f, 0.0f), 2.0f, 1.0f, 4.0f));
    
    
    // 메인 카메라
    MainCam = Camera::Create(mainChar);
    m_cameraControl = false;

    
    // Meshes
    CharMesh = Mesh::CreateBox();
    TileMesh = Mesh::CreateBox();


    // Materials => 메쉬의 색을 나타내는 텍스처들
    CharMat = Material::Create();
    CharMat->diffuse = Texture::CreateFromImage(Image::Load("./image/container2.png").get());
    CharMat->specular = Texture::CreateFromImage(Image::Load("./image/container2_specular.png").get());
    CharMat->shininess = 64.0f;
    CharMesh->SetMaterial(CharMat);


    FloorMat = Material::Create();
    FloorMat->diffuse = Texture::CreateFromImage(Image::Load("./image/container.jpg").get());
    FloorMat->specular = Texture::CreateFromImage(
                            Image::CreateSingleColorImage(
                                FloorMat->diffuse->GetWidth(), FloorMat->diffuse->GetHeight(), glm::vec4(0.5f, 0.5f, 0.5f, 1.0f)).get()
                        );
    TileMesh->SetMaterial(FloorMat);




    
    // Compute Program 에 필요한 Uniform 변수 중, 한번만 정의되는 것을 한꺼번에 정의
    SetComputeUniformOnce();
    
    // Map Program 에 필요한 Uniform 변수
    SetMapUniformOnce();

    SetAlphaMapUniformOnce();






    // 셰이더 테스트 용 프로그램
    testShader = Shader::CreateFromFile("./shader/test.compute", GL_COMPUTE_SHADER);
    testProgram = Program::Create({testShader});
    testBuffer = Buffer::CreateWithData(GL_SHADER_STORAGE_BUFFER, GL_DYNAMIC_DRAW, NULL, sizeof(unsigned int), 32);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 7, testBuffer->Get());
    block_index = glGetProgramResourceIndex(testProgram->Get(), GL_SHADER_STORAGE_BUFFER, "testBuffer");
    glShaderStorageBlockBinding(testProgram->Get(), block_index, 7);

    unsigned int temp = 0;

    testProgram->Use();
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, testBuffer->Get());
            glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(unsigned int), &(temp));
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);


        glDispatchCompute(1, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, testBuffer->Get());
            auto checkData = (unsigned int*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
                for(int i = 0; i < 11; i++)
                    SPDLOG_INFO("{} th data is {}", i, checkData[i]);
            glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    glUseProgram(0);



    return true;
}



// 변하지 않는 Uniform 변수를 미리 넣어두자
void Context::SetComputeUniformOnce()
{
    // Uniform 변수를 정의하기 전에, program 을 use 해야 한다
    ComputeProgram->Use();
    
        ComputeProgram->SetUniform("TileCount", (unsigned int)tileArr.size());
        ComputeProgram->SetUniform("TileScale", glm::vec3(gameMap.STRIDE, 1.0f, gameMap.STRIDE));
        ComputeProgram->SetUniform("CharScale", glm::vec3(mainChar->xScale, mainChar->yScale, mainChar->zScale));

    glUseProgram(0);
}


void Context::SetMapUniformOnce()
{
    // Map Program 에서 한번만 정의되면 되는 Uniform 변수들
    MapProgram->Use();
        MapProgram->SetUniform("TileScale", glm::vec3(gameMap.STRIDE, 1.0f, gameMap.STRIDE));
    glUseProgram(0);
}


void Context::SetAlphaMapUniformOnce()
{
    AlphaMapProgram->Use();
        AlphaMapProgram->SetUniform("TileScale", glm::vec3(gameMap.STRIDE, 1.0f, gameMap.STRIDE));
    glUseProgram(0);
}



void Context::InitGameMap()
{
    // CPU data => 맵을 구성하는 타일들의 위치를 초기화 한다
    for(unsigned int story = 0; story < gameMap.STORY; story++)
    {
        float Height = (-1.0f) * (story) * gameMap.STRIDE * 2;
        for(unsigned int row = 0; row < gameMap.COUNT; row++)
        {
            for(unsigned int col = 0; col < gameMap.COUNT; col++)
            {
                tileArr.push_back
                (
                    Tile
                    {
                        row * gameMap.STRIDE, Height, col * gameMap.STRIDE, 0.0f
                    }
                    //glm::vec4(row * gameMap.STRIDE, Height, col * gameMap.STRIDE, 0.0f)
                );
            }
        }
    }


    // 실행에 필요한 그룹 수 계산 => 무조건 널널하게 설정
    ComputeGroupNum = ((gameMap.COUNT * gameMap.COUNT * gameMap.STORY) / 32) + 1;
}







// 60FPS 속도로 호출된다
void Context::ProcessInput(GLFWwindow* window)
{
    // 이번 프레임에서 키 입력이 있었는 지 체크
    mainChar->xzMoving = false;
    // 새로운 방향 받을 수 있도록 초기화
    mainChar->xzDir = glm::vec3(0.0f, 0.0f, 0.0f);



    // 움직이는 키를 입력 받는다
    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        mainChar->GetXZDir(MainCam->FrontVec);

    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        mainChar->GetXZDir(-MainCam->FrontVec);

    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        mainChar->GetXZDir(-MainCam->LeftVec);

    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        mainChar->GetXZDir(MainCam->LeftVec);



    // Q, E로 카메라 조종을 컨트롤하자
    if(glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    {
        m_cameraControl = true;

        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        m_prevMousePos.x = (float)xpos;
        m_prevMousePos.y = (float)ypos;
    }
    if(glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    {
        m_cameraControl = false;
    }    
}

void Context::Reshape(int width, int height)
{
    m_width = width;
    m_height = height;

    glViewport(0, 0, m_width, m_height);

    //m_framebuffer = Framebuffer::Create(Texture::Create(m_width, m_height, GL_RGBA));
}

void Context::MouseMove(double x, double y)
{
    if(!m_cameraControl) return;

    auto pos = glm::vec2((float)x, (float)y);
    auto deltaPos = pos - m_prevMousePos;

    MainCam->Rotate(deltaPos);
    m_prevMousePos = pos;
}

void Context::MouseButton(int button, int action, double x, double y)
{
    if(m_cameraControl)
        m_prevMousePos = glm::vec2((float) x, (float) y);
}




void Context::UpdateTiles()
{
    // 현재 함수가 실행되는 시간
    double curtime = glfwGetTime();


    // compute program -> shader 를 이용해서 계산한다
    ComputeProgram->Use();
    
        // 매 프레임마다 달라지는 메인 캐릭터의 위치를 입력한다
        ComputeProgram->SetUniform("MainCharPos", mainChar->Position);


        // 실행하기 전에, output buffer 의 값을 초기화 한다
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, outputBuffer->Get());

            auto outputdata = (OutputData *)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
                // 메인 캐릭터 충돌 유무 초기화
                outputdata->HasCharCollision = -1.0f;
            glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);


            // compute shader 실행
            glDispatchCompute(ComputeGroupNum, 1, 1);
            // 배리어 => 모든 스레드가 연산이 끝날 때까지 기다린다?
            glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);


            // 실행한 후, 연산 결과를 확인하기 위해 버퍼에 다시 접근한다
            outputdata = (OutputData *)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_WRITE);
                
                // 먼저 메인 캐릭터의 충돌유무를 확인한다
                if(outputdata->HasCharCollision == +1.0f)
                    mainChar->yStop();
                else
                    mainChar->OnAir();

                
                // 충돌을 감지한 타일이 있는 지 조사한다
                if(outputdata->HasWriteColTile == +1.0f)
                {
                    // 충돌한 것으로 추측되는 타일 인덱스를, 현재 시간과 함께 큐에 넣는다
                    IndexQueue.push_back({outputdata->WriteColTile, curtime});

                        // 큐에 넣은 인덱스를 compute shader 에 알린다
                        outputdata->HasInformColTile = +1.0f;
                        outputdata->InformColTile = outputdata->WriteColTile;
                        
                    // collision 을 다시 받을 수 있도록 초기화 한다
                    outputdata->HasWriteColTile = -1.0f;
                }

                // 큐에 들어간 타일들 중에서, 큐의 가장 앞에 있는 타일의 시간이 다 지났는 지 확인한다
                if(!IndexQueue.empty() && curtime - IndexQueue.front().second > LimitTime)
                {
                    // shader 에 해당 타일이 사라졌다는 것을 알린다
                    outputdata->HasInformDisTile = +1.0f;
                    outputdata->InformDisTile = IndexQueue.front().first;
                    
                    // 큐에서 제거
                    IndexQueue.pop_front();
                }
            
            glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    glUseProgram(0);
}






void Context::Render()
{
    /****************************************************************/
    if(ImGui::Begin("ImGui"))
    {
        ImGui::Checkbox("Camera Control", &(m_cameraControl));

        if(ImGui::CollapsingHeader("Character Setting", ImGuiTreeNodeFlags_DefaultOpen))
        {
            /* XZ 이동 */
            ImGui::DragFloat("Move Speed", &(mainChar->MoveSpeed), 0.01f, 0.0f);
            ImGui::DragFloat("Yaw Angle Tick", &(mainChar->YawAngleTick), 0.01f, 0.0f);
            /* Y 축 이동 */
            ImGui::DragFloat("Gravity", &(mainChar->Gravity), 0.01f, 1.0f);
            /* 점프 */
            ImGui::DragFloat("Jump Power", &(mainChar->JumpPower), 0.01f, 0.0f);
            /* 대쉬 */
            ImGui::DragFloat("Dash Power", &(mainChar->DashPower), 0.01f, 0.0f); 
        }

        if(ImGui::CollapsingHeader("Camera Setting", ImGuiTreeNodeFlags_DefaultOpen))
        {
            /* 카메라 피벗 */
            ImGui::DragFloat("Cam Pivot Min Limit", &(MainCam->CamToCharMinLimit), 0.01f);
            ImGui::DragFloat("Cam Pivot Max Limit", &(MainCam->CamToCharMaxLimit), 0.01f);
            /* 카메라 이동 */
            ImGui::DragFloat("Cam Move Speed", &(MainCam->CameraMoveSpeed), 0.01f);
            /* 카메라 회전 */
            ImGui::DragFloat("Yaw Rot Speed", &(MainCam->yawRotSpeed), 0.001f, 0.0f, 1.0f);
            ImGui::DragFloat("Pitch Rot Speed", &(MainCam->pitchRotSpeed), 0.001f, 0.0f, 1.0f);
            ImGui::DragFloat2("Pitch Rot Limit", glm::value_ptr(MainCam->pitchRotLimit), 0.001f);
            /* 카메라 거리 */
            ImGui::DragFloat("XZ Distance", &(MainCam->xzDist), 0.01f);
            ImGui::DragFloat("Y Distance", &(MainCam->yDist), 0.01f);
        }
        
        if (ImGui::CollapsingHeader("Light Setting", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::DragFloat3("l.position", glm::value_ptr(m_light.position), 0.01f);
            ImGui::DragFloat3("l.direction", glm::value_ptr(m_light.direction), 0.01f);
            ImGui::DragFloat2("l.cutoff", glm::value_ptr(m_light.cutoff), 0.5f, 0.0f, 90.0f);
            ImGui::ColorEdit3("l.ambient", glm::value_ptr(m_light.ambient));
            ImGui::ColorEdit3("l.diffuse", glm::value_ptr(m_light.diffuse));
            ImGui::ColorEdit3("l.specular", glm::value_ptr(m_light.specular));
            ImGui::DragFloat("l.distance", &m_light.distance, 0.5f, 0.0f, 3000.0f);
        }

    }
    
    ImGui::End();
    /****************************************************************/
        
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
 
    
    
    // Light Setting
    {
        CharProgram->SetUniform("viewPos", m_cameraPos);

        CharProgram->SetUniform("light.position", m_light.position);
        CharProgram->SetUniform("light.direction", m_light.direction);
        CharProgram->SetUniform("light.cutoff", glm::vec2(
                                                // Intensity 가 100% 인 각도
                                                cosf(glm::radians(m_light.cutoff[0])),
                                                // 약하더라도 조금 빛을 받을 수 있는 각도 범위
                                                cosf(glm::radians(m_light.cutoff[0] + m_light.cutoff[1]))));
        CharProgram->SetUniform("light.ambient", m_light.ambient);
        CharProgram->SetUniform("light.diffuse", m_light.diffuse);
        CharProgram->SetUniform("light.specular", m_light.specular);
        CharProgram->SetUniform("light.attenuation", GetAttenuationCoeff(m_light.distance));


        MapProgram->SetUniform("viewPos", m_cameraPos);

        MapProgram->SetUniform("light.position", m_light.position);
        MapProgram->SetUniform("light.direction", m_light.direction);
        MapProgram->SetUniform("light.cutoff", glm::vec2(
                                            cosf(glm::radians(m_light.cutoff[0])),
                                            cosf(glm::radians(m_light.cutoff[0] + m_light.cutoff[1]))));
        MapProgram->SetUniform("light.ambient", m_light.ambient);
        MapProgram->SetUniform("light.diffuse", m_light.diffuse);
        MapProgram->SetUniform("light.specular", m_light.specular);
        //MapProgram->SetUniform("light.attenuation", GetAttenuationCoeff(m_light.distance));
    }






/***** 물리 연산 진행 *****/

    /* 
        메인 캐릭터 입장에서 생각해보면

        움직이고 = Move()
            XZ 움직임
                xzMoving == true 면
                    이번에 키를 입력받았다는 거니까
                    Rotate 를 한다
                
                xzMoving == false 면
                    키 입력이 없다, xz 평면에서 움직이지 않는다
                    할 거 없음

            Y 움직임
                이전 높이 저장해두고
                ySpeed <- 점프 입력에 의해서 바뀌어 있을 수도 있는 상황
                중력에 의한 속도 변경
            
            XYZ 방향으로 이동

        충돌판정한다
            하나라도 충돌이 있으면
                ySpeed = 0 으로 만들어 버리고
                점프 초기화
                groundHeight 초기화
                이전 높이로 돌아가기
     */


    // 메인 캐릭터는 이동시키고
    mainChar->Move();

    // 그에 따른 타일들의 상태를 갱신한다
    UpdateTiles();

    // 카메라가 상자를 따라가게 한다
    MainCam->SetPosition();
    





    // 카메라 설정, 위치 등을 확정
    auto projection = glm::perspective
    (
        glm::radians(45.0f),
        (float)m_width / (float)m_height,
        0.01f, 100.0f
    );

    auto view = glm::lookAt
    (
        MainCam->Position,
        MainCam->Position + MainCam->Direction,
        glm::vec3(0.0f, 1.0f, 0.0f)
    );





/***** Draw Call *****/


    // Main Player Draw
    CharProgram->Use();
        auto modelTransform = glm::translate(glm::mat4(1.0f), mainChar->Position) *
                            glm::mat4(glm::vec4(mainChar->LeftVec, 0.0f), 
                                        glm::vec4(mainChar->UpVec, 0.0f),
                                        glm::vec4(mainChar->FrontVec, 0.0f), 
                                        glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)) *
                            glm::scale(glm::mat4(1.0f), glm::vec3(mainChar->xScale, mainChar->yScale, mainChar->zScale));
        
        auto transform = projection * view * modelTransform;
        CharProgram->SetUniform("transform", transform);
        CharProgram->SetUniform("modelTransform", modelTransform);

        CharMesh->Draw(CharProgram.get());
    glUseProgram(0);





    
    // Game Map Draw
  
    // 일단 tileBuffer 에 저장된 모든 데이터를 GPU Instancing 으로 그려내기
    MapProgram->Use();
        transform = projection * view;
        MapProgram->SetUniform("transform", transform);

        TileMesh->GPUInstancingDraw(MapProgram.get(), tileArr.size());
    glUseProgram(0);



    /* 
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        여기서 std map 을 새로 만들고
        
        Unordered Map 을 iterate 하면서 
        카메라 - 타일 사이 거리 체크, 
        std multi map 에 거리를 key, 타일 정보를 value 로 해서 집어 넣는다

        이후 multimap 을 iterate 하면서
        draw call 진행
        멀리 있는 반투명 타일부터 draw 를 진행한다
     */
    
    std::map<float, glm::vec4, std::greater<float>> AlphaTiles{};
    double curTime = glfwGetTime();

                // deque<pair<unsigned int, double>>
    for(auto i : IndexQueue)
    {
        glm::vec4 tilePos = glm::vec4(tileArr[i.first].xpos, tileArr[i.first].ypos, tileArr[i.first].zpos, (float)(curTime - i.second) / LimitTime);

        // 카메라와 타일 사이의 거리를 구한다
        float len = glm::distance(MainCam->Position, glm::vec3(tilePos.x, tilePos.y, tilePos.z));

        // Map 에 집어 넣는다
        AlphaTiles[len] = tilePos;
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    AlphaMapProgram->Use();
        AlphaMapProgram->SetUniform("transform", transform);
        
        // 카메라 거리가 긴 타일 부터 draw call 시작
        for(auto i : AlphaTiles)
        {
            AlphaMapProgram->SetUniform("TilePos", glm::vec3(i.second.x, i.second.y, i.second.z));
            AlphaMapProgram->SetUniform("TimeRatio", i.second.w);



            TileMesh->Draw(AlphaMapProgram.get());
        }
    glUseProgram(0);
    glDisable(GL_BLEND);

}