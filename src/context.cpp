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
    m_program = Program::Create("./shader/lighting.vs", "./shader/lighting.fs");
    if(!m_program)
        return false;

    MapProgram = Program::Create("./shader/Map/map.vs", "./shader/Map/map.fs");

    
    // Main Player
    MainBox = Player::Create();
    MainBox->m_mesh = Mesh::CreateBox();
    MainBox->CreateSetMaterial("./image/container2.png", "./image/container2_specular.png", 64.0f);    


    // 바닥 메쉬, 머터리얼
    floorPtr = Mesh::CreatePlane();
    floorMat = Material::Create();
    floorMat->diffuse = Texture::CreateFromImage(Image::Load("./image/container.jpg").get());
    floorMat->specular = Texture::CreateFromImage(Image::CreateSingleColorImage(
        floorMat->diffuse->GetWidth(), floorMat->diffuse->GetHeight(), glm::vec4(0.5f, 0.5f, 0.5f, 1.0f)).get()
    );
    floorMat->shininess = 64.0f;
    floorPtr->SetMaterial(floorMat);



    // 맵 정보
    curGroundHeight = MainBox->Position.y;

    MapInfo[glm::vec2(0.0f, 0.0f)].push(-2.0f);
    MapInfo[glm::vec2(0.0f, 0.0f)].push(-4.0f);




    // 메인 카메라
    MainCam = Camera::Create(MainBox);


    return true;
}







void Context::ProcessInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        MainBox->MoveXZ(GLFW_KEY_W);

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        MainBox->MoveXZ(GLFW_KEY_S);
        
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        MainBox->MoveXZ(GLFW_KEY_D);

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        MainBox->MoveXZ(GLFW_KEY_A);
}

void Context::Reshape(int width, int height)
{
    m_width = width;
    m_height = height;

    glViewport(0, 0, m_width, m_height);

    m_framebuffer = Framebuffer::Create(Texture::Create(m_width, m_height, GL_RGBA));
}

void Context::MouseMove(double x, double y)
{
    if(!m_cameraControl) return;

    auto pos = glm::vec2((float)x, (float)y);
    auto deltaPos = pos - m_prevMousePos;


    MainBox->Rotate(deltaPos.x);
    MainCam->Rotate(deltaPos.y);

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
        if(ImGui::CollapsingHeader("Player Setting", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::DragFloat("Gravity", &(MainBox->Acceleration.y), 0.001f, -5.0f, -0.001f);
            ImGui::DragFloat("Jump Power", &(MainBox->JumpPower), 0.01f, 0.0f, 2.0f);
            ImGui::DragFloat("Move Speed", &(MainBox->MoveSpeed), 0.01f, 0.0f, 1.0f);
            ImGui::DragFloat("Rot Speed", &(MainBox->RotSpeed), 0.01f, 0.0f, 1.0f);
        }

        if(ImGui::CollapsingHeader("Camera Setting", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::DragFloat("XZ Distance", &(MainCam->xzDist), 0.1f);
            ImGui::DragFloat("Y Distance", &(MainCam->yDist), 0.1f);
            ImGui::DragFloat("Cam Rot Speed", &(MainCam->rotSpeed), 0.1f, 0.0f, 1.0f);
            ImGui::DragFloat2("Cam Rot Limit", glm::value_ptr(MainCam->rotLimit), 0.1f);
        }
        

        ImGui::Checkbox("Camera Control", &(m_cameraControl));
    }
    

    ImGui::End();

    /****************************************************************/
    
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
 
 

    
    
    // Light Setting
    {
        m_program->SetUniform("viewPos", m_cameraPos);

        m_program->SetUniform("light.position", m_light.position);
        m_program->SetUniform("light.direction", m_light.direction);
        m_program->SetUniform("light.cutoff", glm::vec2(
                                            cosf(glm::radians(m_light.cutoff[0])),
                                            cosf(glm::radians(m_light.cutoff[0] + m_light.cutoff[1]))));
        m_program->SetUniform("light.attenuation", GetAttenuationCoeff(m_light.distance));
        m_program->SetUniform("light.ambient", m_light.ambient);
        m_program->SetUniform("light.diffuse", m_light.diffuse);
        m_program->SetUniform("light.specular", m_light.specular);


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




    // 상자의 현재 위치 => 대응되는 바닥을 찾아야 한다
    glm::vec2 MatchedFloorPos { ((int)MainBox->Position.x) / 10, ((int)MainBox->Position.z) / 10 };
    float groundPos = (MapInfo[MatchedFloorPos].empty()) ? -10.0f : MapInfo[MatchedFloorPos].front();

    if(groundPos < curGroundHeight)
        curGroundHeight = groundPos;
    else
    {
        while (!MapInfo[MatchedFloorPos].empty()
                && MapInfo[MatchedFloorPos].front() > curGroundHeight)
        {
            MapInfo[MatchedFloorPos].pop();
        }
    }


    // 상자를 낙하 시키고
    MainBox->MoveY(curGroundHeight);           // y = 0 높이에서 정지
    
    // 카메라가 상자를 따라가게 한다
    MainCam->SetPosition(curGroundHeight);     // y = 0 높이에서 정지한다는 걸 알려준다
    // 카메라 방향 설정
    MainCam->SetDirection();





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



    // Main Player Draw
    auto modelTransform = glm::translate(glm::mat4(1.0f), MainBox->Position) *
                          glm::mat4(glm::vec4(MainBox->LeftVec, 0.0f), 
                                    glm::vec4(MainBox->UpVec, 0.0f),
                                    glm::vec4(MainBox->FrontVec, 0.0f), 
                                    glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    
    auto transform = projection * view * modelTransform;

    m_program->Use();
    m_program->SetUniform("transform", transform);
    m_program->SetUniform("modelTransform", modelTransform);

    MainBox->m_mesh->Draw(m_program.get());




    MapProgram->Use();


        // 바닥 1 Draw
        modelTransform =
            glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -2.0f, 0.0f)) *
            glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)) *
            glm::scale(glm::mat4(1.0f), glm::vec3(10.0f, 10.0f, 10.0f));
        transform = projection * view * modelTransform;
        if(MainBox->OnGround)
            MapProgram->SetUniform("contact", true);
        else
            MapProgram->SetUniform("contact", false);

        MapProgram->SetUniform("transform", transform);
        MapProgram->SetUniform("modelTransform", modelTransform);

        floorPtr->Draw(MapProgram.get());

        // 바닥 2 Draw
        modelTransform =
            glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -4.0f, 0.0f)) *
            glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)) *
            glm::scale(glm::mat4(1.0f), glm::vec3(10.0f, 10.0f, 10.0f));
        transform = projection * view * modelTransform;

        if(MainBox->OnGround)
            MapProgram->SetUniform("contact", true);
        else
            MapProgram->SetUniform("contact", false);

        MapProgram->SetUniform("transform", transform);
        MapProgram->SetUniform("modelTransform", modelTransform);
        
        floorPtr->Draw(MapProgram.get());



    m_program->Use();
        modelTransform =
            glm::translate(glm::mat4(1.0f), b2.Position) *
            glm::scale(glm::mat4(1.0f), glm::vec3(b2.xScale, b2.yScale, b2.zScale));
        transform = projection * view * modelTransform;


        m_program->SetUniform("transform", transform);
        m_program->SetUniform("modelTransform", modelTransform);
        
        MainBox->m_mesh->Draw(m_program.get());
}