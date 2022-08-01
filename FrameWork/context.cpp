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


    // Main Character �׸��� Program ����
        CharProgram = Program::Create("./shader/Character/character.vs", "./shader/Character/character.fs");
        if(!CharProgram) return false;


    // Map �׸��� Program ���� <= GPU Instancing
        MapProgram = Program::Create("./shader/Map/map.vs", "./shader/Map/map.fs");
        if(!MapProgram) return false;


    // Compute Program ����
        // compute shader �� ���� ���� ������ ����, ���δ�
        ComputeShader = Shader::CreateFromFile("./shader/Compute/TileCheck.compute", GL_COMPUTE_SHADER);
        ComputeProgram = Program::Create({ComputeShader});
        if(!ComputeProgram) return false;


    // �������� Map �� �׸��� Program ����
        AlphaMapProgram = Program::Create("./shader/Map/alphamap.vs", "./shader/Map/alphamap.fs");
        if(!AlphaMapProgram) return false;




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

    


    // ���� ĳ����
    mainChar = CharacterPtr(new Character(glm::vec3(0.0f, 10.0f, 0.0f), 2.0f, 1.0f, 4.0f));
    
    
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
                        row * gameMap.STRIDE, Height, col * gameMap.STRIDE, 0.0f, story
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

        ImGui::Checkbox("Update_Tiles", &(Update_Tiles));

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
    // �ϴ� tileBuffer �� ����� ��� �����͸� GPU Instancing ���� �׷�����
    MapProgram->Use();
        transform = projection * view;
        MapProgram->SetUniform("transform", transform);

        TileMesh->GPUInstancingDraw(MapProgram.get(), tileArr.size());
    glUseProgram(0);






    /* 
        ���⼭ std map �� ���� �����
        
        deque �� iterate �ϸ鼭 
        ī�޶� - Ÿ�� ���� �Ÿ� üũ, 
        std map �� �Ÿ��� key, ( Ÿ�� ��ġ + �ð� ���� )�� value �� �ؼ� ���� �ִ´�
            ī�޶�~Ÿ�� �Ÿ��� �� �� -> ����� �� ������ ���ĵǰ� �Ѵ�

        ���� map �� iterate �ϸ鼭
        draw call ����
        �ָ� �ִ� ������ Ÿ�Ϻ��� draw �� �����Ѵ�
     */
    std::map<float, glm::vec4, std::greater<float>> AlphaTiles{};
    double curTime = glfwGetTime();

                // deque<pair<unsigned int, double>>
    for(auto i : IndexQueue)
    {
        // Ÿ�Ͽ� ���� ����             Ÿ�� ��ġ                                                             Ÿ���� ������� ������ ������� �ð��� ���Ҵ� ��
        glm::vec4 tileInfo = glm::vec4(tileArr[i.first].xpos, tileArr[i.first].ypos, tileArr[i.first].zpos, (float)(curTime - i.second) / LimitTime);

        // ī�޶�� Ÿ�� ������ �Ÿ��� ���Ѵ�
        float len = glm::distance(MainCam->Position, glm::vec3(tileInfo.x, tileInfo.y, tileInfo.z));

        // ī�޶���� �Ÿ��� key ��, Map �� ���� �ִ´�
        AlphaTiles[len] = tileInfo;
    }


    // ���� �ϼ��� Map �� Iterate �ϸ鼭, �浹 �� ������� �ִ� ������ Ÿ�ϵ��� �������Ѵ�
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    AlphaMapProgram->Use();
        transform = projection * view;  // ����
        AlphaMapProgram->SetUniform("transform", transform);
        
        // ī�޶� �Ÿ��� �� Ÿ�� ���� draw call ����
        for(auto i : AlphaTiles)
        {
            AlphaMapProgram->SetUniform("TilePos", glm::vec3(i.second.x, i.second.y, i.second.z));
            AlphaMapProgram->SetUniform("TimeRatio", i.second.w);

            TileMesh->Draw(AlphaMapProgram.get());
        }
    glUseProgram(0);
    glDisable(GL_BLEND);

}