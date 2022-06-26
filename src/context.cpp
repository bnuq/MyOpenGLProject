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

    
    // Main Player
    MainBox = Player::Create();
    MainBox->m_mesh = Mesh::CreateBox();
    MainBox->CreateSetMaterial("./image/container2.png", "./image/container2_specular.png", 64.0f);    


    // ¹Ù´Ú
    floorPtr = Mesh::CreatePlane();
    floorMat = Material::Create();
    floorMat->diffuse = Texture::CreateFromImage(Image::Load("./image/container.jpg").get());
    floorMat->specular = Texture::CreateFromImage(Image::CreateSingleColorImage(
        floorMat->diffuse->GetWidth(), floorMat->diffuse->GetHeight(), glm::vec4(0.5f, 0.5f, 0.5f, 1.0f)).get()
    );
    floorMat->shininess = 64.0f;
    floorPtr->SetMaterial(floorMat);


    // ¸ÞÀÎ Ä«¸Þ¶ó
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
    //if(!m_cameraControl) return;

    auto pos = glm::vec2((float)x, (float)y);
    auto deltaPos = pos - m_prevMousePos;

    MainBox->Rotate(deltaPos);
    //MainCam->Rotate(deltaPos);


    m_cameraYaw -= deltaPos.x * m_cameraRotSpeed;
    m_cameraPitch -= deltaPos.y * m_cameraRotSpeed;


    if (m_cameraYaw < 0.0f)   m_cameraYaw += 360.0f;
    if (m_cameraYaw > 360.0f) m_cameraYaw -= 360.0f;

    if (m_cameraPitch > 89.0f)  m_cameraPitch = 89.0f;
    if (m_cameraPitch < -89.0f) m_cameraPitch = -89.0f;


    m_prevMousePos = pos;
}


void Context::MouseButton(int button, int action, double x, double y)
{
    if (button == GLFW_MOUSE_BUTTON_RIGHT)  // ?š°ì¸? ?‚¤ ?´ë¦?
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
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
 
 
    m_cameraFront =
        glm::rotate(glm::mat4(1.0f),
            glm::radians(m_cameraYaw), glm::vec3(0.0f, 1.0f, 0.0f)) *
        glm::rotate(glm::mat4(1.0f),
            glm::radians(m_cameraPitch), glm::vec3(1.0f, 0.0f, 0.0f)) *
        glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);

    
    auto projection = glm::perspective
    (
        glm::radians(45.0f),
        (float)m_width / (float)m_height,
        0.01f, 100.0f
    );


//    MainCam->Attach();
    // auto view = glm::lookAt
    // (
    //     MainCam->Position,
    //     MainBox->Position,
    //     m_cameraUp
    // );

    



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
    MainCam->GetPosition();


    
    auto modelTransform = glm::translate(glm::mat4(1.0f), MainBox->Position) *
                          glm::mat4(glm::vec4(MainBox->LeftVec, 0.0f), 
                                    glm::vec4(MainBox->UpVec, 0.0f),
                                    glm::vec4(MainBox->FrontVec, 0.0f), 
                                    glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));


    //auto CameraPos = glm::vec3(modelTransform * glm::vec4(MainCam->Position, 1.0f));
    
    auto view = glm::lookAt
    (
        MainCam->Position,
        MainBox->Position, //MainCam->Position + MainBox->FrontVec, //CameraPos + m_cameraFront,
        m_cameraUp
    );

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