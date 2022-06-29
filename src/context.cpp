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

    // Programs
    CharProgram = Program::Create("./shader/lighting.vs", "./shader/lighting.fs");
    if(!CharProgram)
        return false;

    MapProgram = Program::Create("./shader/Map/map.vs", "./shader/Map/map.fs");

    
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

    

    // GameMap 을 만든다
    GameMap.push_back(FloorPtr(new Floor(glm::vec3(0.0f, 1.0f, 3.0f))));
    GameMap.push_back(FloorPtr(new Floor(glm::vec3(3.0f, 1.0f, 3.0f))));
    GameMap.push_back(FloorPtr(new Floor(glm::vec3(0.0f, 0.0f, -3.0f), 1.0f, 2.0f, 1.0f)));
    GameMap.push_back(FloorPtr(new Floor(glm::vec3(0.0f, -2.0f, 0.0f), 200.0f, 0.5f, 200.0f)));   // 바닥
    




    


    return true;
}






void Context::ProcessInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        mainChar->Move(GLFW_KEY_W);

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        mainChar->Move(GLFW_KEY_S);
        
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        mainChar->Move(GLFW_KEY_D);

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        mainChar->Move(GLFW_KEY_A);
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

    //mainChar->Rotate(deltaPos.x);
    MainCam->Rotate(deltaPos);

    m_prevMousePos = pos;
}

void Context::MouseButton(int button, int action, double x, double y)
{
    if (button == GLFW_MOUSE_BUTTON_RIGHT)
    {
        if (action == GLFW_PRESS)
        {
            m_prevMousePos = glm::vec2((float)x, (float)y);
            m_cameraControl = true;
        }
        else if (action == GLFW_RELEASE)
        {
            m_cameraControl = false;
        }
    }
}



void Context::Render()
{
    /****************************************************************/
    

    if(ImGui::Begin("ImGui"))
    {
        if(ImGui::CollapsingHeader("Character Setting", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::DragFloat("Gravity", &(mainChar->Acceleration.y), 0.0001f, -5.0f, -0.001f);
            ImGui::DragFloat("Jump Power", &(mainChar->JumpPower), 0.0001f, 0.0f, 2.0f);
            ImGui::DragFloat("Move Speed", &(mainChar->MoveSpeed), 0.0001f, 0.0f, 1.0f);
            ImGui::DragFloat("Char Yaw Rot Speed", &(mainChar->RotSpeed), 0.0001f, 0.0f, 1.0f);
        }

        if(ImGui::CollapsingHeader("Camera Setting", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::DragFloat("XZ Distance", &(MainCam->xzDist), 0.01f);
            ImGui::DragFloat("Y Distance", &(MainCam->yDist), 0.01f);
            ImGui::DragFloat("Yaw Rot Speed", &(MainCam->yawRotSpeed), 0.001f, 0.0f, 1.0f);
            ImGui::DragFloat("Pitch Rot Speed", &(MainCam->pitchRotSpeed), 0.001f, 0.0f, 1.0f);
            ImGui::DragFloat2("Pitch Rot Limit", glm::value_ptr(MainCam->pitchRotLimit), 0.001f);
        }
        

        ImGui::Checkbox("Camera Control", &(m_cameraControl));
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
                                            cosf(glm::radians(m_light.cutoff[0])),
                                            cosf(glm::radians(m_light.cutoff[0] + m_light.cutoff[1]))));
        CharProgram->SetUniform("light.attenuation", GetAttenuationCoeff(m_light.distance));
        CharProgram->SetUniform("light.ambient", m_light.ambient);
        CharProgram->SetUniform("light.diffuse", m_light.diffuse);
        CharProgram->SetUniform("light.specular", m_light.specular);


        MapProgram->SetUniform("viewPos", m_cameraPos);

        MapProgram->SetUniform("light.position", m_light.position);
        MapProgram->SetUniform("light.direction", m_light.direction);
        MapProgram->SetUniform("light.cutoff", glm::vec2(
                                            cosf(glm::radians(m_light.cutoff[0])),
                                            cosf(glm::radians(m_light.cutoff[0] + m_light.cutoff[1]))));
        MapProgram->SetUniform("light.attenuation", GetAttenuationCoeff(m_light.distance));
        MapProgram->SetUniform("light.ambient", m_light.ambient);
        MapProgram->SetUniform("light.diffuse", m_light.diffuse);
        MapProgram->SetUniform("light.specular", m_light.specular);
    }






/***** 물리 연산 진행 *****/

    // 일단 무조건 Y축 방향으로 움직인다
    auto TempPos = mainChar->Position;
    mainChar->Fall();

    // 하나라도 충돌이 있었는 지 확인한다
    bool AnyCollision = false;
    for(auto aFloor : GameMap)
    {
        if(mainChar->Collide(aFloor))
        {
            AnyCollision = true;
            
            aFloor->collision = true;
            break;
        }
    }

    // 충돌이 하나라도 있었다면, 초기화를 하고 위치를 되돌린다
    if(AnyCollision)
    {
        mainChar->Stay(TempPos);
    }


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
    CharProgram->Use();
    for(auto aFloor : GameMap)
    {
        modelTransform =
            glm::translate(glm::mat4(1.0f), aFloor->Position) *
            glm::scale(glm::mat4(1.0f), glm::vec3(aFloor->xScale, aFloor->yScale, aFloor->zScale));
        transform = projection * view * modelTransform;

        CharProgram->SetUniform("transform", transform);
        CharProgram->SetUniform("modelTransform", modelTransform);

        FloorMesh->Draw(CharProgram.get());
    }


        


    // 테스트를 위한 바닥 그리기
    
}