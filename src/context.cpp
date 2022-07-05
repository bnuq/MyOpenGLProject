#include "context.h"
#include <vector>


ContextUPtr Context::Create()                                  
{
    auto context = ContextUPtr(new Context());
    if (!context->Init())
        return nullptr;
    return std::move(context);
}


bool Context::Init()
{
    glClearColor(0.1f, 0.2f, 0.3f, 0.0f);

    // Main Character 그리는 Program
    CharProgram = Program::Create("./shader/Character/character.vs", "./shader/Character/character.fs");
    if(!CharProgram) return false;

    // Map 그리는 Program
    MapProgram = Program::Create("./shader/Map/map.vs", "./shader/Map/map.fs");
    if(!MapProgram) return false;

    
    
    InitComputeProgram();

    /* GPU 로 넘길 데이터 넘기는 과정 */
    InitGameMap();
    InitSSBOs();

    ConnectShaderAndSSBO();
        

        


    
    // Meshes
    CharMesh = Mesh::CreateBox();
    FloorMesh = Mesh::CreateBox();


    // Materials
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
    FloorMesh->SetMaterial(FloorMat);



    // 메인 캐릭터
    mainChar = CharacterPtr(new Character(glm::vec3(0.0f, 10.0f, 0.0f), 2.0f, 1.0f, 4.0f));
    
    // 메인 카메라
    MainCam = Camera::Create(mainChar);
    m_cameraControl = false;

    

/*********************************************************************************/
    /* GameMap 초기화 */    
    GameMap.resize(gameMap.STORY);

    for(int story = 0; story < gameMap.STORY; story++)
    {
        float Height = (-gameMap.STRIDE) * story;

        for(int row = 0; row < gameMap.COUNT; row++)
        {
            for(int col = 0; col < gameMap.COUNT; col++)
            {
                GameMap[story].push_back(Floor::Create(glm::vec3(row * gameMap.STRIDE, Height, col * gameMap.STRIDE),gameMap.STRIDE, 1, gameMap.STRIDE));
            }
        }
    }


/*********************************************************************************/
  


    /* compute program 을 이용하는 부분 */
    ComputeProgram->Use();
        ComputeProgram->SetUniform("TileCount", (unsigned int)tileArr.size());
        glDispatchCompute(1, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

        /* compute shader 에서 작성한 값에 다시 접근할 수 있다 */
        void* ptr = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_WRITE);
        if(ptr == NULL)
            SPDLOG_INFO("glMapBuffer return NULL");
        if(ptr != NULL)
        {
            SPDLOG_INFO("glMapBuffer returns not NULL");

            Tile* tilePtr = (Tile*)ptr;

            for(int i = 0; i < 32; i++)
            {
                SPDLOG_INFO("{}, {}, {}", tilePtr[i].position.x, tilePtr[i].position.y, tilePtr[i].position.z);
            }
        }
    glUseProgram(0);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    return true;
}



void Context::InitComputeProgram()
{
    /* compute shader => 직접 생성한 다음 */
    ComputeShader = Shader::CreateFromFile("./shader/Compute/TileCheck.compute", GL_COMPUTE_SHADER);
    /* compute program 에 compute shader 붙이기 */
    ComputeProgram = Program::Create({ComputeShader});

    // 변하지 않는 Uniform 변수를 미리 넣어두자
    ComputeProgram->SetUniform("TileCount", (unsigned int)tileArr.size());
    ComputeProgram->SetUniform("TileScale", glm::vec3(gameMap.STRIDE, 0.5f, gameMap.STRIDE));
}

void Context::InitGameMap()
{
    // CPU data => 맵을 구성하는 타일들의 위치를 초기화 한다
    for(int story = 0; story < gameMap.STORY; story++)
    {
        float Height = (-story) * gameMap.STRIDE;
        for(int row = 0; row < gameMap.COUNT; row++)
        {
            for(int col = 0; col < gameMap.COUNT; col++)
            {
                tileArr.push_back
                (
                    Tile{glm::vec4(row * gameMap.STRIDE, Height, col * gameMap.STRIDE, 0.0f),
                        glm::vec4(0.0f, 0.0f, 0.0f, 0.0f)}
                );
            }
        }
    }
}

void Context::InitSSBOs()
{
    // SSBO 버퍼를 생성 <= GPU
    // CPU 데이터를 GPU 에 저장
    tileBuffer = Buffer::CreateWithData(GL_SHADER_STORAGE_BUFFER, GL_DYNAMIC_DRAW,
                                        tileArr.data(), sizeof(Tile), tileArr.size());

    // output 데이터를 담을 버퍼
    outputBuffer = Buffer::CreateWithData(GL_SHADER_STORAGE_BUFFER, GL_DYNAMIC_DRAW,
                                        NULL, sizeof(unsigned int), 1);


    // SSBO 버퍼를 Binding Point 에 연결하고
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, tile_binding,   tileBuffer->Get());
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, output_binding, outputBuffer->Get());
}



// shader 와 Binding Point 를 연결한다
void Context::ConnectShaderAndSSBO()
{
    auto block_index = glGetProgramResourceIndex(ComputeShader->Get(), GL_SHADER_STORAGE_BUFFER, "TileBuffer");
    glShaderStorageBlockBinding(ComputeShader->Get(), block_index, tile_binding);

    block_index = glGetProgramResourceIndex(ComputeShader->Get(), GL_SHADER_STORAGE_BUFFER, "OutputBuffer");
    glShaderStorageBlockBinding(ComputeShader->Get(), block_index, output_binding);
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


    // 메인 캐릭터는 움직이고
    mainChar->Move();



    // 하나라도 충돌이 있었는 지 확인한다
    bool AnyCollision = false;

    // 상자의 모든 레벨에 대해서 조사
    for(auto aFloor : GameMap)
    {
        for(int i = 0; i < aFloor.size(); i++)
        {
            // 해당 Floor 가 이미 사라졌다면 다음으로 넘어간다
            // 이미 사라진 바닥이라면, 충돌을 초기화 한다
            if(aFloor[i]->Disappear) continue;

            // 이미 충돌이 일어났던 바닥이라면 시간을 확인한다
            if(aFloor[i]->collision)
            {
                // 시간을 체크했는데, 일정 시간이 지났다면 => 이제 없어져야 한다
                // 사라진 바닥이라고 표시를 한다
                if(aFloor[i]->CheckTime(glfwGetTime()))
                {
                    aFloor[i]->Disappear = true;
                }
            }
            

            // 어떤 Floor 와 이번에 충돌이 발생
            if(mainChar->Collide(aFloor[i]))
            {
                // 충돌이 하나라도 발생하면 일단 무조건 멈춰
                AnyCollision = true;
                
                if(aFloor[i]->collision == false)
                {
                    // Floor 에 충돌을 기록하고 시간을 세팅한다
                    aFloor[i]->SetTime(glfwGetTime());
                }
            }
        }
    }

    // 충돌이 하나라도 있었다면, 낙하를 초기화를 하고 위치를 되돌린다
    if(AnyCollision)
        mainChar->yStop();
    else
        mainChar->OnAir();


    // 카메라가 상자를 따라가게 한다
    MainCam->SetPosition();
    





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




    // Game Map Draw
    MapProgram->Use();
    for(auto aFloor : GameMap)
    {
        for(auto f : aFloor)
        {
            // 사라진 Floor 는 그리지 않는다
            if(f->Disappear) continue;


            // Floor 의 위치, 스케일
            modelTransform =
                glm::translate(glm::mat4(1.0f), f->Position) *
                glm::scale(glm::mat4(1.0f), glm::vec3(f->xScale, f->yScale, f->zScale));
            transform = projection * view * modelTransform;

            // Uniform for Vertex Shader
            MapProgram->SetUniform("transform", transform);
            MapProgram->SetUniform("modelTransform", modelTransform);

            // Uniform for Fragment Shader
            MapProgram->SetUniform("Collision", f->collision);

            FloorMesh->Draw(MapProgram.get());
        }
    }
}