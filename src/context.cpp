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

    // Main Character �׸��� Program
    CharProgram = Program::Create("./shader/Character/character.vs", "./shader/Character/character.fs");
    if(!CharProgram) return false;

    // Map �׸��� Program
    MapProgram = Program::Create("./shader/Map/map.vs", "./shader/Map/map.fs");
    if(!MapProgram) return false;

    
    
    InitComputeProgram();

    /* GPU �� �ѱ� ������ �ѱ�� ���� */
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



    // ���� ĳ����
    mainChar = CharacterPtr(new Character(glm::vec3(0.0f, 10.0f, 0.0f), 2.0f, 1.0f, 4.0f));
    
    // ���� ī�޶�
    MainCam = Camera::Create(mainChar);
    m_cameraControl = false;

    

/*********************************************************************************/
    /* GameMap �ʱ�ȭ */    
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
  


    /* compute program �� �̿��ϴ� �κ� */
    ComputeProgram->Use();
        ComputeProgram->SetUniform("TileCount", (unsigned int)tileArr.size());
        glDispatchCompute(1, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

        /* compute shader ���� �ۼ��� ���� �ٽ� ������ �� �ִ� */
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
    /* compute shader => ���� ������ ���� */
    ComputeShader = Shader::CreateFromFile("./shader/Compute/TileCheck.compute", GL_COMPUTE_SHADER);
    /* compute program �� compute shader ���̱� */
    ComputeProgram = Program::Create({ComputeShader});

    // ������ �ʴ� Uniform ������ �̸� �־����
    ComputeProgram->SetUniform("TileCount", (unsigned int)tileArr.size());
    ComputeProgram->SetUniform("TileScale", glm::vec3(gameMap.STRIDE, 0.5f, gameMap.STRIDE));
}

void Context::InitGameMap()
{
    // CPU data => ���� �����ϴ� Ÿ�ϵ��� ��ġ�� �ʱ�ȭ �Ѵ�
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
    // SSBO ���۸� ���� <= GPU
    // CPU �����͸� GPU �� ����
    tileBuffer = Buffer::CreateWithData(GL_SHADER_STORAGE_BUFFER, GL_DYNAMIC_DRAW,
                                        tileArr.data(), sizeof(Tile), tileArr.size());

    // output �����͸� ���� ����
    outputBuffer = Buffer::CreateWithData(GL_SHADER_STORAGE_BUFFER, GL_DYNAMIC_DRAW,
                                        NULL, sizeof(unsigned int), 1);


    // SSBO ���۸� Binding Point �� �����ϰ�
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, tile_binding,   tileBuffer->Get());
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, output_binding, outputBuffer->Get());
}



// shader �� Binding Point �� �����Ѵ�
void Context::ConnectShaderAndSSBO()
{
    auto block_index = glGetProgramResourceIndex(ComputeShader->Get(), GL_SHADER_STORAGE_BUFFER, "TileBuffer");
    glShaderStorageBlockBinding(ComputeShader->Get(), block_index, tile_binding);

    block_index = glGetProgramResourceIndex(ComputeShader->Get(), GL_SHADER_STORAGE_BUFFER, "OutputBuffer");
    glShaderStorageBlockBinding(ComputeShader->Get(), block_index, output_binding);
}





// 60FPS �ӵ��� ȣ��ȴ�
void Context::ProcessInput(GLFWwindow* window)
{
    // �̹� �����ӿ��� Ű �Է��� �־��� �� üũ
    mainChar->xzMoving = false;
    // ���ο� ���� ���� �� �ֵ��� �ʱ�ȭ
    mainChar->xzDir = glm::vec3(0.0f, 0.0f, 0.0f);



    // �����̴� Ű�� �Է� �޴´�
    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        mainChar->GetXZDir(MainCam->FrontVec);

    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        mainChar->GetXZDir(-MainCam->FrontVec);

    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        mainChar->GetXZDir(-MainCam->LeftVec);

    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        mainChar->GetXZDir(MainCam->LeftVec);



    // Q, E�� ī�޶� ������ ��Ʈ������
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
            /* XZ �̵� */
            ImGui::DragFloat("Move Speed", &(mainChar->MoveSpeed), 0.01f, 0.0f);
            ImGui::DragFloat("Yaw Angle Tick", &(mainChar->YawAngleTick), 0.01f, 0.0f);
            /* Y �� �̵� */
            ImGui::DragFloat("Gravity", &(mainChar->Gravity), 0.01f, 1.0f);
            /* ���� */
            ImGui::DragFloat("Jump Power", &(mainChar->JumpPower), 0.01f, 0.0f);
            /* �뽬 */
            ImGui::DragFloat("Dash Power", &(mainChar->DashPower), 0.01f, 0.0f); 
        }

        if(ImGui::CollapsingHeader("Camera Setting", ImGuiTreeNodeFlags_DefaultOpen))
        {
            /* ī�޶� �ǹ� */
            ImGui::DragFloat("Cam Pivot Min Limit", &(MainCam->CamToCharMinLimit), 0.01f);
            ImGui::DragFloat("Cam Pivot Max Limit", &(MainCam->CamToCharMaxLimit), 0.01f);
            /* ī�޶� �̵� */
            ImGui::DragFloat("Cam Move Speed", &(MainCam->CameraMoveSpeed), 0.01f);
            /* ī�޶� ȸ�� */
            ImGui::DragFloat("Yaw Rot Speed", &(MainCam->yawRotSpeed), 0.001f, 0.0f, 1.0f);
            ImGui::DragFloat("Pitch Rot Speed", &(MainCam->pitchRotSpeed), 0.001f, 0.0f, 1.0f);
            ImGui::DragFloat2("Pitch Rot Limit", glm::value_ptr(MainCam->pitchRotLimit), 0.001f);
            /* ī�޶� �Ÿ� */
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
                                                // Intensity �� 100% �� ����
                                                cosf(glm::radians(m_light.cutoff[0])),
                                                // ���ϴ��� ���� ���� ���� �� �ִ� ���� ����
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






/***** ���� ���� ���� *****/

    /* 
        ���� ĳ���� ���忡�� �����غ���

        �����̰� = Move()
            XZ ������
                xzMoving == true ��
                    �̹��� Ű�� �Է¹޾Ҵٴ� �Ŵϱ�
                    Rotate �� �Ѵ�
                
                xzMoving == false ��
                    Ű �Է��� ����, xz ��鿡�� �������� �ʴ´�
                    �� �� ����

            Y ������
                ���� ���� �����صΰ�
                ySpeed <- ���� �Է¿� ���ؼ� �ٲ�� ���� ���� �ִ� ��Ȳ
                �߷¿� ���� �ӵ� ����
            
            XYZ �������� �̵�

        �浹�����Ѵ�
            �ϳ��� �浹�� ������
                ySpeed = 0 ���� ����� ������
                ���� �ʱ�ȭ
                groundHeight �ʱ�ȭ
                ���� ���̷� ���ư���
     */


    // ���� ĳ���ʹ� �����̰�
    mainChar->Move();



    // �ϳ��� �浹�� �־��� �� Ȯ���Ѵ�
    bool AnyCollision = false;

    // ������ ��� ������ ���ؼ� ����
    for(auto aFloor : GameMap)
    {
        for(int i = 0; i < aFloor.size(); i++)
        {
            // �ش� Floor �� �̹� ������ٸ� �������� �Ѿ��
            // �̹� ����� �ٴ��̶��, �浹�� �ʱ�ȭ �Ѵ�
            if(aFloor[i]->Disappear) continue;

            // �̹� �浹�� �Ͼ�� �ٴ��̶�� �ð��� Ȯ���Ѵ�
            if(aFloor[i]->collision)
            {
                // �ð��� üũ�ߴµ�, ���� �ð��� �����ٸ� => ���� �������� �Ѵ�
                // ����� �ٴ��̶�� ǥ�ø� �Ѵ�
                if(aFloor[i]->CheckTime(glfwGetTime()))
                {
                    aFloor[i]->Disappear = true;
                }
            }
            

            // � Floor �� �̹��� �浹�� �߻�
            if(mainChar->Collide(aFloor[i]))
            {
                // �浹�� �ϳ��� �߻��ϸ� �ϴ� ������ ����
                AnyCollision = true;
                
                if(aFloor[i]->collision == false)
                {
                    // Floor �� �浹�� ����ϰ� �ð��� �����Ѵ�
                    aFloor[i]->SetTime(glfwGetTime());
                }
            }
        }
    }

    // �浹�� �ϳ��� �־��ٸ�, ���ϸ� �ʱ�ȭ�� �ϰ� ��ġ�� �ǵ�����
    if(AnyCollision)
        mainChar->yStop();
    else
        mainChar->OnAir();


    // ī�޶� ���ڸ� ���󰡰� �Ѵ�
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
            // ����� Floor �� �׸��� �ʴ´�
            if(f->Disappear) continue;


            // Floor �� ��ġ, ������
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