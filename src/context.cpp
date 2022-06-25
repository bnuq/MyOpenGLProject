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

    MainBox = Player::Create();
    MainBox->m_mesh = Mesh::CreateBox();
    
    auto TempMat = Material::Create();
    TempMat->diffuse = Texture::CreateFromImage(Image::Load("./image/container2.png").get());
    TempMat->specular = Texture::CreateFromImage(Image::Load("./image/container2_specular.png").get());
    TempMat->shininess = 64.0f;
    MainBox->m_mesh->SetMaterial(std::move(TempMat));


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

// 마우?�� ?��?��?�� ?���? 카메?�� ?��?��각을 ?��?��
void Context::MouseMove(double x, double y)
{
    // 마우?�� ?���? �? ?���? ?��?��?�� ?���? ?��?���?, �??���? 주�?? ?��?��?��
    if(!m_cameraControl) return;

    // ?��?�� 마우?�� 커서 ?���? => ?��?���? 공간?��?��
    auto pos = glm::vec2((float)x, (float)y);

    // 마우?��?�� ???직임 계산
    // Context 멤버 �??���? ?��?��?��, ?��?�� 마우?�� 커서?�� ?��치�?? 기억?�� ?��?��?��
    auto deltaPos = pos - m_prevMousePos;

    // ?��면에?�� x �? ?��?�� => 카메?��?�� y �? 기�?? ?��?��
    m_cameraYaw -= deltaPos.x * m_cameraRotSpeed;
    // ?��면에?�� y �? ?��?�� => 카메?��?�� x �? 기�?? ?��?��
    m_cameraPitch -= deltaPos.y * m_cameraRotSpeed;

    // 카메?��?�� ?��?�� 각도�? 조정
    if (m_cameraYaw < 0.0f)   m_cameraYaw += 360.0f;
    if (m_cameraYaw > 360.0f) m_cameraYaw -= 360.0f;

    if (m_cameraPitch > 89.0f)  m_cameraPitch = 89.0f;
    if (m_cameraPitch < -89.0f) m_cameraPitch = -89.0f;

    // ?���? 갱신
    m_prevMousePos = pos;
}

// 마우?�� ?���??�� 처리
void Context::MouseButton(int button, int action, double x, double y)
{
    if (button == GLFW_MOUSE_BUTTON_RIGHT)  // ?���? ?�� ?���?
    {
        if (action == GLFW_PRESS)
        {
            // 마우?�� 조작 ?��?�� ?��?��?�� ?��?�� 마우?�� 커서 ?���? ????��
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
    ?��면에 그리?�� ?��?��?�� 진행?��?�� ?��?��
    �? ?��?��?��마다 ?��출된?��
    Draw Call ?�� ?���?
 */
void Context::Render()
{
 
    /*
        ?��????�� 출력?��?�� Frame Buffer ?��?��

        Color Buffer 초기?�� <- ?��?��?��?�� Clear Color �? 초기?��
        Depth Buffer 초기?��
     */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    // 깊이 ?��?��?��, 깊이 체크�? 진행?��겠다
    glEnable(GL_DEPTH_TEST);
    //glDisable(GL_DEPTH_TEST); // => UI �? 그릴 ?�� ?��?��

    /*
        마우?�� ?��?�� => 카메?�� ?��?��각이 ?��?���?
            �?금까�? ?��?��받�?? Camera Parameter �? 바탕?���?
            Camera Front 벡터�? ?��?�� 게산
     */
    m_cameraFront =
        // y �? ?��?��
        glm::rotate(glm::mat4(1.0f),
            glm::radians(m_cameraYaw), glm::vec3(0.0f, 1.0f, 0.0f)) *
        // x �? ?��?��
        glm::rotate(glm::mat4(1.0f),
            glm::radians(m_cameraPitch), glm::vec3(1.0f, 0.0f, 0.0f)) *
        // 기본?��?��로는 (0, 0, -1) ?�� 바라보는 벡터????�� -
        glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);

    /*
        ?��?�� �??�� ?��?�� 구하�?
     */
    auto projection = glm::perspective
    (
        glm::radians(45.0f),
        (float)m_width / (float)m_height,
        0.01f, 100.0f
    );

    // lookAt ?��?���? ?��?��?��?��, �? �??��?�� 구한?��
    auto view = glm::lookAt
    (
        m_cameraPos,                 // ?��?�� 카메?�� ?���?
        m_cameraPos + m_cameraFront, // ?��치에 벡터�? ?��?��?��, 바라보는 좌표�? 구한?��
        m_cameraUp                   // 카메?��?�� UP ?���? 좌표
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



        
    MainBox->MoveY(-2.0f);
    
    auto modelTransform = glm::translate(glm::mat4(1.0f), MainBox->Position);
    auto transform = projection * view * modelTransform;

    m_program->SetUniform("transform", transform);
    m_program->SetUniform("modelTransform", modelTransform);
//    player.matPtr->SetToProgram(m_program.get());

    MainBox->m_mesh->Draw(m_program.get());



    modelTransform =
        glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -2.0f, 0.0f)) *
        glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)) *
        glm::scale(glm::mat4(1.0f), glm::vec3(3.0f, 3.0f, 3.0f));
    transform = projection * view * modelTransform;

    m_program->SetUniform("transform", transform);
    m_program->SetUniform("modelTransform", modelTransform);
    
    floorPtr->Draw(m_program.get());


  
}