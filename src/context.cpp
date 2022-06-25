#include "context.h"
#include <vector>

ContextUPtr Context::Create()
{
    auto context = ContextUPtr(new Context());

    SPDLOG_INFO("Start Initialization");

    if (!context->Init())
        return nullptr;

    SPDLOG_INFO("Finish Initialization");

    return std::move(context);
}

// 화면을 그리는 데 필요한 모든 요소들을 초기화
bool Context::Init()
{
    // 기본 배경색인 Color Buffer 초기화
    glClearColor(0.1f, 0.2f, 0.3f, 0.0f);

    // Programs
    m_program = Program::Create("./shader/lighting.vs", "./shader/lighting.fs");
    if(!m_program)
        return false;


    // Player Mesh, Material 설정
    player.meshPtr = Mesh::CreateBox();
    player.matPtr = Material::Create();
    player.matPtr->diffuse = Texture::CreateFromImage(Image::Load("./image/container2.png").get());
    player.matPtr->specular = Texture::CreateFromImage(Image::Load("./image/container2_specular.png").get());
    player.matPtr->shininess = 64.0f;
 
    player.meshPtr->SetMaterial(player.matPtr);

    floorPtr = Mesh::CreatePlane();
    floorMat = Material::Create();
    floorMat->diffuse = Texture::CreateFromImage(Image::Load("./image/container.jpg").get());
    floorMat->specular = Texture::CreateFromImage(Image::CreateSingleColorImage(
        floorMat->diffuse->GetWidth(), floorMat->diffuse->GetHeight(), glm::vec4(0.5f, 0.5f, 0.5f, 1.0f)).get()
    );
    floorMat->shininess = 64.0f;
    floorPtr->SetMaterial(floorMat);

    return true;
}

void Context::PlayerJump()
{
    SPDLOG_INFO("Context :: Playjump");
    if(!player.jumping)
    {
        player.velocity += 0.2f;
        player.jumping = true;
    }
}

void Context::PlayerGround()
{
    SPDLOG_INFO("Context :: Playground");
    player.jumping = false;
}







// 키보드를 통해 입력된 키 처리
void Context::ProcessInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        player.position += player.moveSpeed * player.frontDir;

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        player.position -= player.moveSpeed * player.frontDir;
    
    
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        player.position += player.moveSpeed * player.rightDir;

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        player.position -= player.moveSpeed * player.rightDir;


    // if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS
    //     && !player.jumping && !player.falling)
    // {
    //     SPDLOG_INFO("SPACE KEY pressed {} {}", player.jumping, player.falling);
    //     player.netForce += 10.0f;
    //     player.jumping = true;
    // }
    //glfwSetKeyCallback(window, key_callback);
    
}

// 윈도우 크기가 변할 때
void Context::Reshape(int width, int height)
{
    m_width = width;
    m_height = height;

    // 뷰 포트의 크기를 윈도우 크기와 동일하게 재조정
    glViewport(0, 0, m_width, m_height);

    m_framebuffer = Framebuffer::Create(Texture::Create(m_width, m_height, GL_RGBA));
}

// 마우스 입력에 따른 카메라 회전각을 세팅
void Context::MouseMove(double x, double y)
{
    // 마우스 오른 쪽 키가 눌러져 있지 않다면, 변화를 주지 않는다
    if(!m_cameraControl) return;

    // 현재 마우스 커서 위치 => 스크린 공간에서
    auto pos = glm::vec2((float)x, (float)y);

    // 마우스의 움직임 계산
    // Context 멤버 변수를 통해서, 이전 마우스 커서의 위치를 기억해 놓는다
    auto deltaPos = pos - m_prevMousePos;

    // 화면에서 x 축 이동 => 카메라는 y 축 기준 회전
    m_cameraYaw -= deltaPos.x * m_cameraRotSpeed;
    // 화면에서 y 축 이동 => 카메라는 x 축 기준 회전
    m_cameraPitch -= deltaPos.y * m_cameraRotSpeed;

    // 카메라의 회전 각도를 조정
    if (m_cameraYaw < 0.0f)   m_cameraYaw += 360.0f;
    if (m_cameraYaw > 360.0f) m_cameraYaw -= 360.0f;

    if (m_cameraPitch > 89.0f)  m_cameraPitch = 89.0f;
    if (m_cameraPitch < -89.0f) m_cameraPitch = -89.0f;

    // 위치 갱신
    m_prevMousePos = pos;
}

// 마우스 클릭을 처리
void Context::MouseButton(int button, int action, double x, double y)
{
    if (button == GLFW_MOUSE_BUTTON_RIGHT)  // 우측 키 클릭
    {
        if (action == GLFW_PRESS)
        {
            // 마우스 조작 시작 시점에 현재 마우스 커서 위치 저장
            m_prevMousePos = glm::vec2((float)x, (float)y);
            m_cameraControl = true;
        }
        else if (action == GLFW_RELEASE)
        {
            m_cameraControl = false;
        }
    }
}

/*
    화면에 그리는 작업을 진행하는 함수
    매 프레임마다 호출된다
    Draw Call 을 호출
 */
void Context::Render()
{
 
    /*
        픽셀을 출력하는 Frame Buffer 에서

        Color Buffer 초기화 <- 설정해둔 Clear Color 로 초기화
        Depth Buffer 초기화
     */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    // 깊이 테스트, 깊이 체크를 진행하겠다
    glEnable(GL_DEPTH_TEST);
    //glDisable(GL_DEPTH_TEST); // => UI 를 그릴 때 사용

    /*
        마우스 입력 => 카메라 회전각이 달라짐
            지금까지 입력받은 Camera Parameter 를 바탕으로
            Camera Front 벡터를 다시 게산
     */
    m_cameraFront =
        // y 축 회전
        glm::rotate(glm::mat4(1.0f),
            glm::radians(m_cameraYaw), glm::vec3(0.0f, 1.0f, 0.0f)) *
        // x 축 회전
        glm::rotate(glm::mat4(1.0f),
            glm::radians(m_cameraPitch), glm::vec3(1.0f, 0.0f, 0.0f)) *
        // 기본적으로는 (0, 0, -1) 을 바라보는 벡터였다 -
        glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);

    /*
        투영 변환 다시 구하고
     */
    auto projection = glm::perspective
    (
        glm::radians(45.0f),
        (float)m_width / (float)m_height,
        0.01f, 100.0f
    );

    // lookAt 함수를 이용해서, 뷰 변환을 구한다
    auto view = glm::lookAt
    (
        m_cameraPos,                 // 현재 카메라 위치
        m_cameraPos + m_cameraFront, // 위치에 벡터를 더해서, 바라보는 좌표를 구한다
        m_cameraUp                   // 카메라의 UP 위치 좌표
    );



    m_program->Use();
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



        
    player.velocity += ((player.onGround) ? 0.0f : player.acc);
    auto tempPos = player.position + glm::vec3(0.0f, 1.0f, 0.0f) * player.velocity;
    if(tempPos.y > -2.0f)
    {
        player.onGround = false;
        player.position = tempPos;
    }
    else
    { 
        player.onGround = true;
        player.velocity = 0.0f;

        player.position.y = -2.0f;
        player.jumping = false;
    }
    
    

    

    
        
    





    
    auto modelTransform = glm::translate(glm::mat4(1.0f), player.position);
    auto transform = projection * view * modelTransform;

    m_program->SetUniform("transform", transform);
    m_program->SetUniform("modelTransform", modelTransform);
//    player.matPtr->SetToProgram(m_program.get());

    player.meshPtr->Draw(m_program.get());



    modelTransform =
        glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -2.0f, 0.0f)) *
        glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)) *
        glm::scale(glm::mat4(1.0f), glm::vec3(3.0f, 3.0f, 3.0f));
    transform = projection * view * modelTransform;

    m_program->SetUniform("transform", transform);
    m_program->SetUniform("modelTransform", modelTransform);
    
    floorPtr->Draw(m_program.get());


  
}