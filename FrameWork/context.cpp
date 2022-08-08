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
    // Color Buffer => Clear Color 설정
    glClearColor(0.1f, 0.2f, 0.3f, 0.0f);
    // Stencil Buffer => Clear 값 설정
    glClearStencil(0);



    // Main Character 그리는 Program 생성
        CharProgram = Program::Create("./shader/Character/character.vs", "./shader/Character/character.fs");
        if(!CharProgram) return false;


    // Map 그리는 Program 생성 <= GPU Instancing
        MapProgram = Program::Create("./shader/Map/map.vs", "./shader/Map/map.fs");
        if(!MapProgram) return false;


    // Compute Program 생성
        // compute shader 를 직접 먼저 생성한 다음, 붙인다
        ComputeShader = Shader::CreateFromFile("./shader/TileCheck/TileCheck.compute", GL_COMPUTE_SHADER);
        ComputeProgram = Program::Create({ComputeShader});
        if(!ComputeProgram) return false;


    // 반투명한 Map 을 그리는 Program 생성
        AlphaMapProgram = Program::Create("./shader/AlphaMap/alphamap.vs", "./shader/AlphaMap/alphamap.fs");
        if(!AlphaMapProgram) return false;


    // Simple Program
        SimpleProgram = Program::Create("./shader/Simple/simple.vs", "./shader/Simple/simple.fs");
        if(!SimpleProgram) return false;





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

    
        // Alpha Map Program => 타일 인덱스만으로 타일 데이터에 접근 => 타일 렌더링
        block_index = glGetProgramResourceIndex(AlphaMapProgram->Get(), GL_SHADER_STORAGE_BUFFER, "TileBuffer");
        glShaderStorageBlockBinding(AlphaMapProgram->Get(), block_index, tile_binding);




    // 메인 캐릭터
    mainChar = CharacterPtr(new Character(glm::vec3(0.0f, 10.0f, 0.0f), 2.0f, 1.0f, 4.0f));


    // 메인 광원
    MainLight = LightUPtr( new Light(mainChar->Position) );
    
    
    // 메인 카메라
    MainCam = Camera::Create(mainChar);
    m_cameraControl = false;

    
    // Meshes
    CharMesh = Mesh::CreateBox();
    TileMesh = Mesh::CreateBox();


    // Materials => 메쉬의 색을 나타내는 텍스처들
    // 메인 캐릭터만 텍스처들을 가진다
    CharMat = Material::Create();
    CharMat->diffuse = Texture::CreateFromImage(Image::Load("./image/container2.png").get());
    CharMat->specular = Texture::CreateFromImage(Image::Load("./image/container2_specular.png").get());
    CharMat->shininess = 64.0f;
    CharMesh->SetMaterial(CharMat);


    
    // 각 프로그램에서 한번만 정의되면 되는, Uniform Variables 를 정의한다
    // Compute Program 에 필요한 Uniform 변수 중, 한번만 정의되는 것을 한꺼번에 정의
    SetComputeUniformOnce();
    // Map Program 에서 한번만 필요한 Uniform 변수들
    SetMapUniformOnce();
    // Alpha Program 에서 한번만 필요한 Uniform 변수들
    SetAlphaMapUniformOnce();




    // shadow map 을 그리기 위한 frame buffer 와 그와 연결된 텍스처를 만들자
    shadow_map_buffer = ShadowMap::Create(1024, 1024);      // shadow map 이 될 텍스처를 생성하고 프레임 버퍼에 연결



    return true;
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
                        row * gameMap.STRIDE, Height, col * gameMap.STRIDE, 0, story
                    }
                );
            }
        }
    }


    // 실행에 필요한 그룹 수 계산 => 무조건 널널하게 설정
    ComputeGroupNum = ((gameMap.COUNT * gameMap.COUNT * gameMap.STORY) / 32) + 1;
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

    // 바뀐 위도우 크기만큼 viewport 를 재설정한다
    glViewport(0, 0, m_width, m_height);
}

void Context::MouseMove(double x, double y)
{
    if(!m_cameraControl) return;

    auto pos = glm::vec2((float)x, (float)y);
    auto deltaPos = pos - m_prevMousePos;

    // 마우스가 움직이는 만큼, 카메라가 회전한다
    MainCam->Rotate(deltaPos);
    m_prevMousePos = pos;
}

// 여기서는 마우스 클릭에 따른 동작이 없다
void Context::MouseButton(int button, int action, double x, double y)
{
    if (button == GLFW_MOUSE_BUTTON_RIGHT) 
    {
        if (action == GLFW_PRESS)    // 우측 키 클릭
            // Tiles 를 업데이트 하는 변수의 값을 반대로 입력한다
            Update_Tiles = !(Update_Tiles);
    }
}



// 매 프레임마다 타일들의 상태를 확인하는 함수
void Context::UpdateTiles()
{
    // 현재 함수가 실행되는 시간
    double curtime = glfwGetTime();


    // compute program -> shader 를 이용해서 계산한다
    ComputeProgram->Use();
    
        // 매 프레임마다 달라지는 메인 캐릭터의 위치를 입력한다
        ComputeProgram->SetUniform("MainCharPos", mainChar->Position);

        // 이전에 계산된 캐릭터 ~ 닿을 것으로 추정되는 바닥 층수를 넘긴다
        ComputeProgram->SetUniform("CharStory", mainChar->CharStory);


        // CPU 와 GPU 사이에서 데이터를 주고 받는, output buffer 를 사용하기 위해서 SSBO 로 Bind 한다
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, outputBuffer->Get());

            // Compute Program 실행 전, output data 를 초기화하기 위해 접근한다
            auto outputdata = (OutputData *)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
                // 메인 캐릭터 충돌 유무 초기화
                outputdata->HasCharCollision = -1.0f;
            glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);


            // compute shader 실행
            glDispatchCompute(ComputeGroupNum, 1, 1);
            // 배리어 => 모든 스레드의 SSBO 접근이 끝날 때까지 기다린다
            glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);


            // 실행한 후, 연산 결과를 확인하기 위해 버퍼에 다시 접근한다
            outputdata = (OutputData *)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_WRITE);
                
                // 먼저 메인 캐릭터의 충돌유무를 확인한다
                if(outputdata->HasCharCollision == +1.0f)   // 메인 캐릭터 충돌 시
                    mainChar->yStop();                      // y 축 방향으로 이동하지 않는다
                else
                    mainChar->OnAir();                      // 충돌하지 않은 경우, 낙하한다

                
                // 타일들의 상태를 바꾸는 경우에만, Compute Program 의 결과를 CPU 에서 반영한다
                if(Update_Tiles)
                {
                    // 이번 프레임에서 충돌을 감지한 타일이 있는 지 조사한다
                    if(outputdata->HasWriteColTile == +1.0f)
                    {
                        // 충돌한 것으로 추측되는 타일 인덱스를, 현재 시간과 함께 큐에 넣는다
                        IndexQueue.push_back( {outputdata->WriteColTile, curtime} );

                            // output buffer 를 통해서, 큐에 넣은 인덱스를 compute shader 에 알린다
                            outputdata->HasInformColTile = +1.0f;
                            outputdata->InformColTile = outputdata->WriteColTile;
                            
                        // collision 을 다시 받을 수 있도록 초기화 한다
                        outputdata->HasWriteColTile = -1.0f;
                    }


                    // 큐에 들어간 타일들 중에서, 큐의 가장 앞에 있는 타일의 시간이 다 지났는 지 확인한다
                    // 순서대로 들어갔기 때문에, 반드시 큐의 앞에서부터 사라질 것이라는 판단
                    if(!IndexQueue.empty() && curtime - IndexQueue.front().second > LimitTime)
                    {
                        // shader 에 해당 타일이 사라졌다는 것을 알린다
                        outputdata->HasInformDisTile = +1.0f;
                        outputdata->InformDisTile = IndexQueue.front().first;
                        
                        // 큐에서 제거
                        // 한번에 하나의 타일만 사라진다
                        IndexQueue.pop_front();
                    }
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
        if(ImGui::CollapsingHeader("Main Character", ImGuiTreeNodeFlags_DefaultOpen))
        {
            /* 위치 확인용 */
            ImGui::DragFloat3("mainChar position", glm::value_ptr(mainChar->Position), 0.01f);
            /* XZ 이동 */
            ImGui::DragFloat("Move Speed", &(mainChar->MoveSpeed), 0.01f, 0.0f);
            /* Y 축 이동 */
            ImGui::DragFloat("Gravity", &(mainChar->Gravity), 0.01f, 1.0f);
            /* 회전 */
            ImGui::DragFloat("Yaw Angle Tick", &(mainChar->YawAngleTick), 0.01f, 0.0f);
            /* 점프 */
            ImGui::DragFloat("Jump Power", &(mainChar->JumpPower), 0.01f, 0.0f);
        }


        if(ImGui::CollapsingHeader("Main Camera", ImGuiTreeNodeFlags_DefaultOpen))
        {
            /* 카메라 피벗 */
            ImGui::DragFloat("Cam Pivot Min Limit", &(MainCam->CamToCharMinLimit), 0.01f, 0.01f);
            ImGui::DragFloat("Cam Pivot Max Limit", &(MainCam->CamToCharMaxLimit), 0.01f, 0.01f);
            /* 카메라 이동 */
            ImGui::Checkbox("Camera Control", &(m_cameraControl));
            ImGui::DragFloat("Cam Move Speed", &(MainCam->CameraMoveSpeed), 0.01f);
            /* 카메라 회전 */
            ImGui::DragFloat("Yaw Rot Speed", &(MainCam->yawRotSpeed), 0.001f, 0.0f, 1.0f);
            ImGui::DragFloat("Pitch Rot Speed", &(MainCam->pitchRotSpeed), 0.001f, 0.0f, 1.0f);
            ImGui::DragFloat2("Pitch Rot Limit", glm::value_ptr(MainCam->pitchRotLimit), 0.001f);
            /* 카메라 거리 */
            ImGui::DragFloat("XZ Distance", &(MainCam->xzDist));
            ImGui::DragFloat("Y Distance", &(MainCam->yDist));
                    
        }
        
        
        if (ImGui::CollapsingHeader("Main Light", ImGuiTreeNodeFlags_DefaultOpen))
        {
            /* 광원의 초기 위치 */
            ImGui::DragFloat3("light position", glm::value_ptr(MainLight->Position), 0.01f);
            /* 광원의 방향 */
            ImGui::DragFloat3("light direction", glm::value_ptr(MainLight->Direction), 0.01f);
            /* 광원 이동 */
            ImGui::DragFloat("light move speed", &MainLight->LightMoveSpeed, 0.01f, 0.0f);
            /* 광원 색깔 */
            ImGui::ColorEdit3("light ambient", glm::value_ptr(MainLight->ambient));
            ImGui::ColorEdit3("light diffuse", glm::value_ptr(MainLight->diffuse));
            ImGui::ColorEdit3("light specular", glm::value_ptr(MainLight->specular));
            /* 광원 클립 공간 */
            ImGui::DragFloat("light ortho minus x", &MainLight->minusX);
            ImGui::DragFloat("light ortho plus x", &MainLight->plusX);

            ImGui::DragFloat("light ortho minus y", &MainLight->minusY);
            ImGui::DragFloat("light ortho plus y", &MainLight->plusY);

            ImGui::DragFloat("light ortho near z", &MainLight->nearZ);
            ImGui::DragFloat("light ortho far z", &MainLight->farZ);
        }

        // 타일 계산 여부 확인
        ImGui::Checkbox("Update_Tiles", &(Update_Tiles));

        // 깊이 맵 확인
        ImGui::Image((ImTextureID)shadow_map_buffer->GetShadowMap()->Get(), ImVec2(256, 256), ImVec2(0, 1), ImVec2(1, 0));

    }
    
    ImGui::End();
    /****************************************************************/
        
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
 
    
    

    



    





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

        
        충돌판정한다 = UpdateTiles()
            하나라도 충돌이 있으면
                ySpeed = 0 으로 만들어 버리고
                점프 초기화
                groundHeight 초기화
                이전 높이로 돌아가기
     */

    // 메인 캐릭터는 이동시키고 => XZ 이동 + Y 이동( 점프  or 대쉬 )
    mainChar->Move();


    // 그에 따른 타일들의 상태를 갱신한다
    // 타일 상태에 따라서 캐릭터의 y 축 움직임이 결정된다
    UpdateTiles();


    // Update 된 캐릭터 위치를 바탕으로, 캐릭터가 닿을 것으로 예상되는 층수를 업데이트한다
    mainChar->CharStory = (unsigned int)((-1.0f) * (mainChar->Position.y / (2 * gameMap.STRIDE)) + 0.5f);
    //SPDLOG_INFO("{}", mainChar->LandStory);



    // 카메라가 캐릭터를 따라가게 한다
    MainCam->SetPosition();

    // 카메라 설정, 위치 등을 확정
    auto projection = glm::perspective
    (
        glm::radians(45.0f),
        (float)m_width / (float)m_height,
        1.0f, 500.0f
    );

    auto view = glm::lookAt
    (
        MainCam->Position,
        MainCam->Position + MainCam->Direction,
        glm::vec3(0.0f, 1.0f, 0.0f)
    );
    


    /* 
        광원을 그리기 전에, 광원의 위치를 갱신하다

        광원이 캐릭터보다 위에 있는데, 캐릭터가 너무 밑으로 내려가는 경우
     */
    MainLight->SetPosition(mainChar->Position);



    // 일단 광원을 그린다
    SimpleProgram->Use();
        auto modelTransform = glm::translate(glm::mat4(1.0f), MainLight->Position);
        auto transform = projection * view * modelTransform;
        // vertex shader
            SimpleProgram->SetUniform("transform", transform);
        // fragment shader
            SimpleProgram->SetUniform("MainColor", MainLight->diffuse);

        CharMesh->Draw(SimpleProgram.get());
    glUseProgram(0);





    /* 
        First Pass => 그림자를 만들기 위해 먼저 광원의 입장에서 렌더링을 진행

        지금 궁금한건, 광원 입장에서 깊이 값만 렌더링할 때 그림자를 만드는 대상인 캐릭터만 렌더링하느냐, 아니면 타일들까지 같이 렌더링 하느냐
            캐릭터에 의한 그림자는 만들고 싶지만
            타일이 타일에 만드는 그림자는 만들고 싶지 않다

        1. 우선 캐릭터만 광원에서 그려보고

        2. 캐릭터 + 불투명한 타일 모두를 그려보자
     */
    // 일단 광원 입장에서 보는 뷰 변환, 프로젝션 변환은 동일하다
    auto lightView = glm::lookAt
    (
        MainLight->Position,
        MainLight->Position + MainLight->Direction,
        glm::vec3(0.0f, 1.0f, 0.0f)
    );
    
    // 나는 무조건 directiona light 라 생각
    auto lightProjection = glm::ortho(
            MainLight->minusX, MainLight->plusX, 
            MainLight->minusY, MainLight->plusY, 
            MainLight->nearZ, MainLight->farZ
        );



    // shadow map 이 그려질 프레임 버퍼를 사용
    shadow_map_buffer->Bind();
        glClear(GL_DEPTH_BUFFER_BIT);
        glCullFace(GL_FRONT);   // 앞면을 컬링

        glViewport
        (
            0, 
            0,
            shadow_map_buffer->GetShadowMap()->GetWidth(),
            shadow_map_buffer->GetShadowMap()->GetHeight()
        );

        SimpleProgram->Use();

            // Main Character Draw
            // Vertex Shader
                modelTransform = glm::translate(glm::mat4(1.0f), mainChar->Position) *
                                    // 메인 캐릭터 좌표계가 되도록 회전하는, 회전 변환
                                    glm::mat4(glm::vec4(mainChar->LeftVec, 0.0f), 
                                                glm::vec4(mainChar->UpVec, 0.0f),
                                                glm::vec4(mainChar->FrontVec, 0.0f), 
                                                glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)) *
                                    glm::scale(glm::mat4(1.0f), glm::vec3(mainChar->xScale, mainChar->yScale, mainChar->zScale));
                
                transform = lightProjection * lightView * modelTransform;
                SimpleProgram->SetUniform("transform", transform);

            // Fragment Shader
                SimpleProgram->SetUniform("MainColor", glm::vec3(1,1,0));

            CharMesh->Draw(SimpleProgram.get());
        glUseProgram(0);

        glCullFace(GL_BACK);
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, m_width, m_height);









    // Game Map Draw
    // 일단 tileBuffer 에 저장된 모든 데이터를 GPU Instancing 으로 그려내기
    MapProgram->Use();
        // Vertex Shader
            // 월드 좌표계 => 클립 좌표계 변환 넘김
            transform = projection * view;
            MapProgram->SetUniform("transform", transform);
            MapProgram->SetUniform("LightTransform", lightProjection * lightView);


        // Fragment Shader
            MapProgram->SetUniform("viewPos", MainCam->Position);

            // Light Setting
            MapProgram->SetUniform("light.position", MainLight->Position);
            MapProgram->SetUniform("light.direction", MainLight->Direction);
            MapProgram->SetUniform("light.ambient", MainLight->ambient);
            MapProgram->SetUniform("light.diffuse", MainLight->diffuse);
            MapProgram->SetUniform("light.specular", MainLight->specular);

            MapProgram->SetUniform("diffRatio", 0.8f);
            
            glActiveTexture(GL_TEXTURE4);
            shadow_map_buffer->GetShadowMap()->Bind();
            MapProgram->SetUniform("shadowMap", 4);

            MapProgram->SetUniform("charStory", mainChar->CharStory);

        TileMesh->GPUInstancingDraw(MapProgram.get(), tileArr.size());
    glUseProgram(0);



    // Main Player Draw
    /* 
        캐릭터를 가릴 수도 있는, 타일들을 먼저 렌더링한 뒤에 캐릭터를 그린다

        메인 캐릭터를 그릴 때, stencil buffer test 를 활성화
        
        무조건 stencil test 는 통과하게 한다
        이후 depth test 는 통과 못하는 경우
            캐릭터가 그려지지 않는 픽셀
            나중에 outline 으로 덮어씌울 것이다
            stencil buffer 값을 0 으로 그대로 유지시킨다

        이후 depth test 까지 통과해서 ~ 캐릭터의 fragment 가 그려지는 경우
            픽셀 부분에 stencil buffer 값 1 을 저장
            stencil buffer 를 통해서, 메인 캐릭터가 그려지는 픽셀 부분을 판단하도록 한다
     */
    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

        glStencilMask(0xFF);                    // stencil buffer 값의 16 비트 모두 사용
        glStencilFunc(GL_ALWAYS, 1, 0xFF);      // stencil test 에 사용되는 함수, 항상 테스트 통과, 1 값과 비교

        CharProgram->Use();                     // stencil test 가 켜진 상태에서 메인 캐릭터를 렌더링한다
            // Vertex Shader
                modelTransform = glm::translate(glm::mat4(1.0f), mainChar->Position) *
                                    // 메인 캐릭터 좌표계가 되도록 회전하는, 회전 변환
                                    glm::mat4(glm::vec4(mainChar->LeftVec, 0.0f), 
                                                glm::vec4(mainChar->UpVec, 0.0f),
                                                glm::vec4(mainChar->FrontVec, 0.0f), 
                                                glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)) *
                                    glm::scale(glm::mat4(1.0f), glm::vec3(mainChar->xScale, mainChar->yScale, mainChar->zScale));
                
                transform = projection * view * modelTransform;
                CharProgram->SetUniform("transform", transform);
                CharProgram->SetUniform("modelTransform", modelTransform);
                CharProgram->SetUniform("LightTransform", lightProjection * lightView);

            
            // Fragment Shader
                CharProgram->SetUniform("viewPos", MainCam->Position);

                // Light Setting
                CharProgram->SetUniform("light.position", MainLight->Position);
                CharProgram->SetUniform("light.direction", MainLight->Direction);
                CharProgram->SetUniform("light.ambient", MainLight->ambient);
                CharProgram->SetUniform("light.diffuse", MainLight->diffuse);
                CharProgram->SetUniform("light.specular", MainLight->specular);

                glActiveTexture(GL_TEXTURE4);
                shadow_map_buffer->GetShadowMap()->Bind();
                CharProgram->SetUniform("shadowMap", 4);

            CharMesh->Draw(CharProgram.get());
        glUseProgram(0);

    // stencil test 를 종료한다
    glDisable(GL_STENCIL_TEST);









    /* 
        여기서 std map 을 새로 만들고
        
        deque 을 iterate => deque 에 저장된 타일 인덱스를 확인
        
        CPU 에 저장된 Tile Array 의 데이터를 이용
        카메라 - 타일 사이 거리 체크, 
            
        std map 에
            거리를 key, 타일 인덱스를 value 로 해서 집어 넣는다
            카메라~타일 거리가 먼 것 -> 가까운 것 순서로 정렬되게 한다

        이후 map 을 iterate 하면서
        순서대로 타일 인덱스를 넘겨 ~ 해당 인덱스 타일에 대해 draw call 진행
        멀리 있는 반투명 타일부터 draw 를 진행한다
     */
    std::map<float, std::pair<unsigned int, double>, std::greater<float>> AlphaTiles{};
    double curTime = glfwGetTime();


    /* 
        deque<pair<unsigned int, double>> IndexQueue

        타일 인덱스와 들어간 시간이 저장되어 있는 큐

        타일의 위치는 변하지 않기 때문에, CPU 에 저장된 타일의 위치 데이터를 이용해도 된다
     */
    for(auto i : IndexQueue)
    {
        // i.first => 타일의 인덱스
        // 카메라와 타일 사이의 거리를 구한다
        float len = glm::distance(MainCam->Position, glm::vec3(tileArr[i.first].xpos, tileArr[i.first].ypos, tileArr[i.first].zpos));

        // 카메라까지 거리를 key 로, IndexQueue 에 들어있던 정보를 집어 넣는다
        AlphaTiles[len] = i;
    }


    // 이제 완성된 Map 을 Iterate 하면서, 충돌 후 사라지고 있는 반투명 타일들을 렌더링한다
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    AlphaMapProgram->Use();
        transform = projection * view;  // 동일
        // vertex shader ~ 모든 타일에 동일
            AlphaMapProgram->SetUniform("transform", transform);
            AlphaMapProgram->SetUniform("LightTransform", lightProjection * lightView);

        // fragment shader ~ 모든 타일에 동일
            glActiveTexture(GL_TEXTURE4);
            shadow_map_buffer->GetShadowMap()->Bind();
            AlphaMapProgram->SetUniform("shadowMap", 4);

            AlphaMapProgram->SetUniform("charStory", mainChar->CharStory);
        
        // 카메라 거리가 긴 타일 부터 draw call 시작
        for(auto i : AlphaTiles)
        {
            unsigned int TileIndex = i.second.first;
            double StoredTime = i.second.second;

            // vertex shader + fragment shader ~ 타일마다 다른다
                AlphaMapProgram->SetUniform("TileIndex", TileIndex);
                AlphaMapProgram->SetUniform("TimeRatio", (float)((curTime - StoredTime) / LimitTime));

            TileMesh->Draw(AlphaMapProgram.get());
        }
    glUseProgram(0);
    glDisable(GL_BLEND);





    // 마지막으로 캐릭터 위에 박스를 그려, 캐릭터가 가려지는 경우 위치도 표시한다
    // outline 그리니까 그림자가 가려서, 빡친다
    /* 
        stencil test 를 다시 활성화
        stencil buffer 에 저장된 값을 읽어와서 1과 같은 지 확인한다
            1 이라면 => 캐릭터가 그려지는 픽셀
                outline 이 가리면 안되니까, 아무것도 하지 않는다
                test fail 조건
            
            1 이 아니라면 => 캐릭터가 안 그려지는 픽셀
                무조건 outline 을 그린다
                depth test 와 관계없이 무조건 그림을 그린다
                    depth test 를 끈다
     */
    glEnable(GL_STENCIL_TEST);                  // stencil test 는 켜고
    glDisable(GL_DEPTH_TEST);                   // depth test 는 끈다

        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);    // stencil test 설정, 이 테스트를 통과해야 color buffer 에 색을 넣게 된다
        
        SimpleProgram->Use();                   // 단순히 색만 넣는 simple program 을 사용

            // vertex shader
                modelTransform = glm::translate(glm::mat4(1.0f), mainChar->Position) *
                                        // 메인 캐릭터 좌표계가 되도록 회전하는, 회전 변환
                                        glm::mat4(glm::vec4(mainChar->LeftVec, 0.0f), 
                                                    glm::vec4(mainChar->UpVec, 0.0f),
                                                    glm::vec4(mainChar->FrontVec, 0.0f), 
                                                    glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)) *
                                        glm::scale(glm::mat4(1.0f), glm::vec3(mainChar->xScale, mainChar->yScale, mainChar->zScale));

                SimpleProgram->SetUniform("transform", projection * view * modelTransform);

            // framgnet shader
                SimpleProgram->SetUniform("MainColor", glm::vec3(0.2f, 0.2f, 0.2f));

            CharMesh->Draw(SimpleProgram.get());

        glUseProgram(0);

    glDisable(GL_STENCIL_TEST);
    glEnable(GL_DEPTH_TEST);

}