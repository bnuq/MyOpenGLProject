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
    // Color Buffer => Clear Color ����
    glClearColor(0.1f, 0.2f, 0.3f, 0.0f);
    // Stencil Buffer => Clear �� ����
    glClearStencil(0);



    // Main Character �׸��� Program ����
        CharProgram = Program::Create("./shader/Character/character.vs", "./shader/Character/character.fs");
        if(!CharProgram) return false;


    // Map �׸��� Program ���� <= GPU Instancing
        MapProgram = Program::Create("./shader/Map/map.vs", "./shader/Map/map.fs");
        if(!MapProgram) return false;


    // Compute Program ����
        // compute shader �� ���� ���� ������ ����, ���δ�
        ComputeShader = Shader::CreateFromFile("./shader/TileCheck/TileCheck.compute", GL_COMPUTE_SHADER);
        ComputeProgram = Program::Create({ComputeShader});
        if(!ComputeProgram) return false;


    // �������� Map �� �׸��� Program ����
        AlphaMapProgram = Program::Create("./shader/AlphaMap/alphamap.vs", "./shader/AlphaMap/alphamap.fs");
        if(!AlphaMapProgram) return false;


    // Simple Program
        SimpleProgram = Program::Create("./shader/Simple/simple.vs", "./shader/Simple/simple.fs");
        if(!SimpleProgram) return false;





    // ��ü ���� ���� �����Ѵ� ==> CPU ���� ������ �ʱ�ȭ
        InitGameMap();
    

    // CPU �����͸� GPU ������ �����ϴ� Shader Storage Buffer Object �� ����
        // Ÿ�� ������ => ���ϴ� Ÿ�ϵ��� �����Ͱ� �״�� �����
        tileBuffer 
            = Buffer::CreateWithData(GL_SHADER_STORAGE_BUFFER, GL_DYNAMIC_DRAW, tileArr.data(), sizeof(Tile), tileArr.size());
        
        // output ������ => CPU �� GPU ���� �ְ�޴� ������
        outputBuffer 
            = Buffer::CreateWithData(GL_SHADER_STORAGE_BUFFER, GL_DYNAMIC_DRAW, &(outputdata), sizeof(OutputData), 1);


    // �� SSBO ���۸� Binding Point �� �����Ѵ�
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, tile_binding,   tileBuffer->Get());
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, output_binding, outputBuffer->Get());


    // SSBO �� ����� ���α׷��� Binding Point �� �����Ѵ�
        // Compute Program => Ÿ�ϵ��� ���¸� ����ϰ�, output data �� ���� ����� �ְ� �޴´�
        auto block_index = glGetProgramResourceIndex(ComputeProgram->Get(), GL_SHADER_STORAGE_BUFFER, "TileBuffer");
        glShaderStorageBlockBinding(ComputeProgram->Get(), block_index, tile_binding);

        block_index = glGetProgramResourceIndex(ComputeProgram->Get(), GL_SHADER_STORAGE_BUFFER, "OutputBuffer");
        glShaderStorageBlockBinding(ComputeProgram->Get(), block_index, output_binding);


        // Map Program => Ÿ�� ������, Ÿ�� �����͸� �ʿ�
        block_index = glGetProgramResourceIndex(MapProgram->Get(), GL_SHADER_STORAGE_BUFFER, "TileBuffer");
        glShaderStorageBlockBinding(MapProgram->Get(), block_index, tile_binding);

    
        // Alpha Map Program => Ÿ�� �ε��������� Ÿ�� �����Ϳ� ���� => Ÿ�� ������
        block_index = glGetProgramResourceIndex(AlphaMapProgram->Get(), GL_SHADER_STORAGE_BUFFER, "TileBuffer");
        glShaderStorageBlockBinding(AlphaMapProgram->Get(), block_index, tile_binding);




    // ���� ĳ����
    mainChar = CharacterPtr(new Character(glm::vec3(0.0f, 10.0f, 0.0f), 2.0f, 1.0f, 4.0f));


    // ���� ����
    MainLight = LightUPtr( new Light(mainChar->Position) );
    
    
    // ���� ī�޶�
    MainCam = Camera::Create(mainChar);
    m_cameraControl = false;

    
    // Meshes
    CharMesh = Mesh::CreateBox();
    TileMesh = Mesh::CreateBox();


    // Materials => �޽��� ���� ��Ÿ���� �ؽ�ó��
    // ���� ĳ���͸� �ؽ�ó���� ������
    CharMat = Material::Create();
    CharMat->diffuse = Texture::CreateFromImage(Image::Load("./image/container2.png").get());
    CharMat->specular = Texture::CreateFromImage(Image::Load("./image/container2_specular.png").get());
    CharMat->shininess = 64.0f;
    CharMesh->SetMaterial(CharMat);


    
    // �� ���α׷����� �ѹ��� ���ǵǸ� �Ǵ�, Uniform Variables �� �����Ѵ�
    // Compute Program �� �ʿ��� Uniform ���� ��, �ѹ��� ���ǵǴ� ���� �Ѳ����� ����
    SetComputeUniformOnce();
    // Map Program ���� �ѹ��� �ʿ��� Uniform ������
    SetMapUniformOnce();
    // Alpha Program ���� �ѹ��� �ʿ��� Uniform ������
    SetAlphaMapUniformOnce();




    // shadow map �� �׸��� ���� frame buffer �� �׿� ����� �ؽ�ó�� ������
    shadow_map_buffer = ShadowMap::Create(1024, 1024);      // shadow map �� �� �ؽ�ó�� �����ϰ� ������ ���ۿ� ����



    return true;
}



void Context::InitGameMap()
{
    // CPU data => ���� �����ϴ� Ÿ�ϵ��� ��ġ�� �ʱ�ȭ �Ѵ�
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


    // ���࿡ �ʿ��� �׷� �� ��� => ������ �γ��ϰ� ����
    ComputeGroupNum = ((gameMap.COUNT * gameMap.COUNT * gameMap.STORY) / 32) + 1;
}



// ������ �ʴ� Uniform ������ �̸� �־����
void Context::SetComputeUniformOnce()
{
    // Uniform ������ �����ϱ� ����, program �� use �ؾ� �Ѵ�
    ComputeProgram->Use();
    
        ComputeProgram->SetUniform("TileCount", (unsigned int)tileArr.size());
        ComputeProgram->SetUniform("TileScale", glm::vec3(gameMap.STRIDE, 1.0f, gameMap.STRIDE));
        ComputeProgram->SetUniform("CharScale", glm::vec3(mainChar->xScale, mainChar->yScale, mainChar->zScale));

    glUseProgram(0);
}


void Context::SetMapUniformOnce()
{
    // Map Program ���� �ѹ��� ���ǵǸ� �Ǵ� Uniform ������
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

    // �ٲ� ������ ũ�⸸ŭ viewport �� �缳���Ѵ�
    glViewport(0, 0, m_width, m_height);
}

void Context::MouseMove(double x, double y)
{
    if(!m_cameraControl) return;

    auto pos = glm::vec2((float)x, (float)y);
    auto deltaPos = pos - m_prevMousePos;

    // ���콺�� �����̴� ��ŭ, ī�޶� ȸ���Ѵ�
    MainCam->Rotate(deltaPos);
    m_prevMousePos = pos;
}

// ���⼭�� ���콺 Ŭ���� ���� ������ ����
void Context::MouseButton(int button, int action, double x, double y)
{
    if (button == GLFW_MOUSE_BUTTON_RIGHT) 
    {
        if (action == GLFW_PRESS)    // ���� Ű Ŭ��
            // Tiles �� ������Ʈ �ϴ� ������ ���� �ݴ�� �Է��Ѵ�
            Update_Tiles = !(Update_Tiles);
    }
}



// �� �����Ӹ��� Ÿ�ϵ��� ���¸� Ȯ���ϴ� �Լ�
void Context::UpdateTiles()
{
    // ���� �Լ��� ����Ǵ� �ð�
    double curtime = glfwGetTime();


    // compute program -> shader �� �̿��ؼ� ����Ѵ�
    ComputeProgram->Use();
    
        // �� �����Ӹ��� �޶����� ���� ĳ������ ��ġ�� �Է��Ѵ�
        ComputeProgram->SetUniform("MainCharPos", mainChar->Position);

        // ������ ���� ĳ���� ~ ���� ������ �����Ǵ� �ٴ� ������ �ѱ��
        ComputeProgram->SetUniform("CharStory", mainChar->CharStory);


        // CPU �� GPU ���̿��� �����͸� �ְ� �޴�, output buffer �� ����ϱ� ���ؼ� SSBO �� Bind �Ѵ�
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, outputBuffer->Get());

            // Compute Program ���� ��, output data �� �ʱ�ȭ�ϱ� ���� �����Ѵ�
            auto outputdata = (OutputData *)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
                // ���� ĳ���� �浹 ���� �ʱ�ȭ
                outputdata->HasCharCollision = -1.0f;
            glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);


            // compute shader ����
            glDispatchCompute(ComputeGroupNum, 1, 1);
            // �踮�� => ��� �������� SSBO ������ ���� ������ ��ٸ���
            glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);


            // ������ ��, ���� ����� Ȯ���ϱ� ���� ���ۿ� �ٽ� �����Ѵ�
            outputdata = (OutputData *)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_WRITE);
                
                // ���� ���� ĳ������ �浹������ Ȯ���Ѵ�
                if(outputdata->HasCharCollision == +1.0f)   // ���� ĳ���� �浹 ��
                    mainChar->yStop();                      // y �� �������� �̵����� �ʴ´�
                else
                    mainChar->OnAir();                      // �浹���� ���� ���, �����Ѵ�

                
                // Ÿ�ϵ��� ���¸� �ٲٴ� ��쿡��, Compute Program �� ����� CPU ���� �ݿ��Ѵ�
                if(Update_Tiles)
                {
                    // �̹� �����ӿ��� �浹�� ������ Ÿ���� �ִ� �� �����Ѵ�
                    if(outputdata->HasWriteColTile == +1.0f)
                    {
                        // �浹�� ������ �����Ǵ� Ÿ�� �ε�����, ���� �ð��� �Բ� ť�� �ִ´�
                        IndexQueue.push_back( {outputdata->WriteColTile, curtime} );

                            // output buffer �� ���ؼ�, ť�� ���� �ε����� compute shader �� �˸���
                            outputdata->HasInformColTile = +1.0f;
                            outputdata->InformColTile = outputdata->WriteColTile;
                            
                        // collision �� �ٽ� ���� �� �ֵ��� �ʱ�ȭ �Ѵ�
                        outputdata->HasWriteColTile = -1.0f;
                    }


                    // ť�� �� Ÿ�ϵ� �߿���, ť�� ���� �տ� �ִ� Ÿ���� �ð��� �� ������ �� Ȯ���Ѵ�
                    // ������� ���� ������, �ݵ�� ť�� �տ������� ����� ���̶�� �Ǵ�
                    if(!IndexQueue.empty() && curtime - IndexQueue.front().second > LimitTime)
                    {
                        // shader �� �ش� Ÿ���� ������ٴ� ���� �˸���
                        outputdata->HasInformDisTile = +1.0f;
                        outputdata->InformDisTile = IndexQueue.front().first;
                        
                        // ť���� ����
                        // �ѹ��� �ϳ��� Ÿ�ϸ� �������
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
            /* ��ġ Ȯ�ο� */
            ImGui::DragFloat3("mainChar position", glm::value_ptr(mainChar->Position), 0.01f);
            /* XZ �̵� */
            ImGui::DragFloat("Move Speed", &(mainChar->MoveSpeed), 0.01f, 0.0f);
            /* Y �� �̵� */
            ImGui::DragFloat("Gravity", &(mainChar->Gravity), 0.01f, 1.0f);
            /* ȸ�� */
            ImGui::DragFloat("Yaw Angle Tick", &(mainChar->YawAngleTick), 0.01f, 0.0f);
            /* ���� */
            ImGui::DragFloat("Jump Power", &(mainChar->JumpPower), 0.01f, 0.0f);
        }


        if(ImGui::CollapsingHeader("Main Camera", ImGuiTreeNodeFlags_DefaultOpen))
        {
            /* ī�޶� �ǹ� */
            ImGui::DragFloat("Cam Pivot Min Limit", &(MainCam->CamToCharMinLimit), 0.01f, 0.01f);
            ImGui::DragFloat("Cam Pivot Max Limit", &(MainCam->CamToCharMaxLimit), 0.01f, 0.01f);
            /* ī�޶� �̵� */
            ImGui::Checkbox("Camera Control", &(m_cameraControl));
            ImGui::DragFloat("Cam Move Speed", &(MainCam->CameraMoveSpeed), 0.01f);
            /* ī�޶� ȸ�� */
            ImGui::DragFloat("Yaw Rot Speed", &(MainCam->yawRotSpeed), 0.001f, 0.0f, 1.0f);
            ImGui::DragFloat("Pitch Rot Speed", &(MainCam->pitchRotSpeed), 0.001f, 0.0f, 1.0f);
            ImGui::DragFloat2("Pitch Rot Limit", glm::value_ptr(MainCam->pitchRotLimit), 0.001f);
            /* ī�޶� �Ÿ� */
            ImGui::DragFloat("XZ Distance", &(MainCam->xzDist));
            ImGui::DragFloat("Y Distance", &(MainCam->yDist));
                    
        }
        
        
        if (ImGui::CollapsingHeader("Main Light", ImGuiTreeNodeFlags_DefaultOpen))
        {
            /* ������ �ʱ� ��ġ */
            ImGui::DragFloat3("light position", glm::value_ptr(MainLight->Position), 0.01f);
            /* ������ ���� */
            ImGui::DragFloat3("light direction", glm::value_ptr(MainLight->Direction), 0.01f);
            /* ���� �̵� */
            ImGui::DragFloat("light move speed", &MainLight->LightMoveSpeed, 0.01f, 0.0f);
            /* ���� ���� */
            ImGui::ColorEdit3("light ambient", glm::value_ptr(MainLight->ambient));
            ImGui::ColorEdit3("light diffuse", glm::value_ptr(MainLight->diffuse));
            ImGui::ColorEdit3("light specular", glm::value_ptr(MainLight->specular));
            /* ���� Ŭ�� ���� */
            ImGui::DragFloat("light ortho minus x", &MainLight->minusX);
            ImGui::DragFloat("light ortho plus x", &MainLight->plusX);

            ImGui::DragFloat("light ortho minus y", &MainLight->minusY);
            ImGui::DragFloat("light ortho plus y", &MainLight->plusY);

            ImGui::DragFloat("light ortho near z", &MainLight->nearZ);
            ImGui::DragFloat("light ortho far z", &MainLight->farZ);
        }

        // Ÿ�� ��� ���� Ȯ��
        ImGui::Checkbox("Update_Tiles", &(Update_Tiles));

        // ���� �� Ȯ��
        ImGui::Image((ImTextureID)shadow_map_buffer->GetShadowMap()->Get(), ImVec2(256, 256), ImVec2(0, 1), ImVec2(1, 0));

    }
    
    ImGui::End();
    /****************************************************************/
        
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
 
    
    

    



    





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

        
        �浹�����Ѵ� = UpdateTiles()
            �ϳ��� �浹�� ������
                ySpeed = 0 ���� ����� ������
                ���� �ʱ�ȭ
                groundHeight �ʱ�ȭ
                ���� ���̷� ���ư���
     */

    // ���� ĳ���ʹ� �̵���Ű�� => XZ �̵� + Y �̵�( ����  or �뽬 )
    mainChar->Move();


    // �׿� ���� Ÿ�ϵ��� ���¸� �����Ѵ�
    // Ÿ�� ���¿� ���� ĳ������ y �� �������� �����ȴ�
    UpdateTiles();


    // Update �� ĳ���� ��ġ�� ��������, ĳ���Ͱ� ���� ������ ����Ǵ� ������ ������Ʈ�Ѵ�
    mainChar->CharStory = (unsigned int)((-1.0f) * (mainChar->Position.y / (2 * gameMap.STRIDE)) + 0.5f);
    //SPDLOG_INFO("{}", mainChar->LandStory);



    // ī�޶� ĳ���͸� ���󰡰� �Ѵ�
    MainCam->SetPosition();

    // ī�޶� ����, ��ġ ���� Ȯ��
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
        ������ �׸��� ����, ������ ��ġ�� �����ϴ�

        ������ ĳ���ͺ��� ���� �ִµ�, ĳ���Ͱ� �ʹ� ������ �������� ���
     */
    MainLight->SetPosition(mainChar->Position);



    // �ϴ� ������ �׸���
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
        First Pass => �׸��ڸ� ����� ���� ���� ������ ���忡�� �������� ����

        ���� �ñ��Ѱ�, ���� ���忡�� ���� ���� �������� �� �׸��ڸ� ����� ����� ĳ���͸� �������ϴ���, �ƴϸ� Ÿ�ϵ���� ���� ������ �ϴ���
            ĳ���Ϳ� ���� �׸��ڴ� ����� ������
            Ÿ���� Ÿ�Ͽ� ����� �׸��ڴ� ����� ���� �ʴ�

        1. �켱 ĳ���͸� �������� �׷�����

        2. ĳ���� + �������� Ÿ�� ��θ� �׷�����
     */
    // �ϴ� ���� ���忡�� ���� �� ��ȯ, �������� ��ȯ�� �����ϴ�
    auto lightView = glm::lookAt
    (
        MainLight->Position,
        MainLight->Position + MainLight->Direction,
        glm::vec3(0.0f, 1.0f, 0.0f)
    );
    
    // ���� ������ directiona light �� ����
    auto lightProjection = glm::ortho(
            MainLight->minusX, MainLight->plusX, 
            MainLight->minusY, MainLight->plusY, 
            MainLight->nearZ, MainLight->farZ
        );



    // shadow map �� �׷��� ������ ���۸� ���
    shadow_map_buffer->Bind();
        glClear(GL_DEPTH_BUFFER_BIT);
        glCullFace(GL_FRONT);   // �ո��� �ø�

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
                                    // ���� ĳ���� ��ǥ�谡 �ǵ��� ȸ���ϴ�, ȸ�� ��ȯ
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
    // �ϴ� tileBuffer �� ����� ��� �����͸� GPU Instancing ���� �׷�����
    MapProgram->Use();
        // Vertex Shader
            // ���� ��ǥ�� => Ŭ�� ��ǥ�� ��ȯ �ѱ�
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
        ĳ���͸� ���� ���� �ִ�, Ÿ�ϵ��� ���� �������� �ڿ� ĳ���͸� �׸���

        ���� ĳ���͸� �׸� ��, stencil buffer test �� Ȱ��ȭ
        
        ������ stencil test �� ����ϰ� �Ѵ�
        ���� depth test �� ��� ���ϴ� ���
            ĳ���Ͱ� �׷����� �ʴ� �ȼ�
            ���߿� outline ���� ����� ���̴�
            stencil buffer ���� 0 ���� �״�� ������Ų��

        ���� depth test ���� ����ؼ� ~ ĳ������ fragment �� �׷����� ���
            �ȼ� �κп� stencil buffer �� 1 �� ����
            stencil buffer �� ���ؼ�, ���� ĳ���Ͱ� �׷����� �ȼ� �κ��� �Ǵ��ϵ��� �Ѵ�
     */
    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

        glStencilMask(0xFF);                    // stencil buffer ���� 16 ��Ʈ ��� ���
        glStencilFunc(GL_ALWAYS, 1, 0xFF);      // stencil test �� ���Ǵ� �Լ�, �׻� �׽�Ʈ ���, 1 ���� ��

        CharProgram->Use();                     // stencil test �� ���� ���¿��� ���� ĳ���͸� �������Ѵ�
            // Vertex Shader
                modelTransform = glm::translate(glm::mat4(1.0f), mainChar->Position) *
                                    // ���� ĳ���� ��ǥ�谡 �ǵ��� ȸ���ϴ�, ȸ�� ��ȯ
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

    // stencil test �� �����Ѵ�
    glDisable(GL_STENCIL_TEST);









    /* 
        ���⼭ std map �� ���� �����
        
        deque �� iterate => deque �� ����� Ÿ�� �ε����� Ȯ��
        
        CPU �� ����� Tile Array �� �����͸� �̿�
        ī�޶� - Ÿ�� ���� �Ÿ� üũ, 
            
        std map ��
            �Ÿ��� key, Ÿ�� �ε����� value �� �ؼ� ���� �ִ´�
            ī�޶�~Ÿ�� �Ÿ��� �� �� -> ����� �� ������ ���ĵǰ� �Ѵ�

        ���� map �� iterate �ϸ鼭
        ������� Ÿ�� �ε����� �Ѱ� ~ �ش� �ε��� Ÿ�Ͽ� ���� draw call ����
        �ָ� �ִ� ������ Ÿ�Ϻ��� draw �� �����Ѵ�
     */
    std::map<float, std::pair<unsigned int, double>, std::greater<float>> AlphaTiles{};
    double curTime = glfwGetTime();


    /* 
        deque<pair<unsigned int, double>> IndexQueue

        Ÿ�� �ε����� �� �ð��� ����Ǿ� �ִ� ť

        Ÿ���� ��ġ�� ������ �ʱ� ������, CPU �� ����� Ÿ���� ��ġ �����͸� �̿��ص� �ȴ�
     */
    for(auto i : IndexQueue)
    {
        // i.first => Ÿ���� �ε���
        // ī�޶�� Ÿ�� ������ �Ÿ��� ���Ѵ�
        float len = glm::distance(MainCam->Position, glm::vec3(tileArr[i.first].xpos, tileArr[i.first].ypos, tileArr[i.first].zpos));

        // ī�޶���� �Ÿ��� key ��, IndexQueue �� ����ִ� ������ ���� �ִ´�
        AlphaTiles[len] = i;
    }


    // ���� �ϼ��� Map �� Iterate �ϸ鼭, �浹 �� ������� �ִ� ������ Ÿ�ϵ��� �������Ѵ�
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    AlphaMapProgram->Use();
        transform = projection * view;  // ����
        // vertex shader ~ ��� Ÿ�Ͽ� ����
            AlphaMapProgram->SetUniform("transform", transform);
            AlphaMapProgram->SetUniform("LightTransform", lightProjection * lightView);

        // fragment shader ~ ��� Ÿ�Ͽ� ����
            glActiveTexture(GL_TEXTURE4);
            shadow_map_buffer->GetShadowMap()->Bind();
            AlphaMapProgram->SetUniform("shadowMap", 4);

            AlphaMapProgram->SetUniform("charStory", mainChar->CharStory);
        
        // ī�޶� �Ÿ��� �� Ÿ�� ���� draw call ����
        for(auto i : AlphaTiles)
        {
            unsigned int TileIndex = i.second.first;
            double StoredTime = i.second.second;

            // vertex shader + fragment shader ~ Ÿ�ϸ��� �ٸ���
                AlphaMapProgram->SetUniform("TileIndex", TileIndex);
                AlphaMapProgram->SetUniform("TimeRatio", (float)((curTime - StoredTime) / LimitTime));

            TileMesh->Draw(AlphaMapProgram.get());
        }
    glUseProgram(0);
    glDisable(GL_BLEND);





    // ���������� ĳ���� ���� �ڽ��� �׷�, ĳ���Ͱ� �������� ��� ��ġ�� ǥ���Ѵ�
    // outline �׸��ϱ� �׸��ڰ� ������, ��ģ��
    /* 
        stencil test �� �ٽ� Ȱ��ȭ
        stencil buffer �� ����� ���� �о�ͼ� 1�� ���� �� Ȯ���Ѵ�
            1 �̶�� => ĳ���Ͱ� �׷����� �ȼ�
                outline �� ������ �ȵǴϱ�, �ƹ��͵� ���� �ʴ´�
                test fail ����
            
            1 �� �ƴ϶�� => ĳ���Ͱ� �� �׷����� �ȼ�
                ������ outline �� �׸���
                depth test �� ������� ������ �׸��� �׸���
                    depth test �� ����
     */
    glEnable(GL_STENCIL_TEST);                  // stencil test �� �Ѱ�
    glDisable(GL_DEPTH_TEST);                   // depth test �� ����

        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);    // stencil test ����, �� �׽�Ʈ�� ����ؾ� color buffer �� ���� �ְ� �ȴ�
        
        SimpleProgram->Use();                   // �ܼ��� ���� �ִ� simple program �� ���

            // vertex shader
                modelTransform = glm::translate(glm::mat4(1.0f), mainChar->Position) *
                                        // ���� ĳ���� ��ǥ�谡 �ǵ��� ȸ���ϴ�, ȸ�� ��ȯ
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