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

// ?λ©΄μ κ·Έλ¦¬? ?° ??? λͺ¨λ  ???€? μ΄κΈ°?
bool Context::Init()
{
    // κΈ°λ³Έ λ°°κ²½??Έ Color Buffer μ΄κΈ°?fgfg€©€·€©€·€©€·€€€©
    glClearColor(0.1f, 0.2f, 0.3f, 0.0f);

    // Programs
    m_program = Program::Create("./shader/lighting.vs", "./shader/lighting.fs");
    if(!m_program)
        return false;


    // Player Mesh, Material ?€? 
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







// ?€λ³΄λλ₯? ?΅?΄ ?? ₯? ?€ μ²λ¦¬
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

// ???° ?¬κΈ°κ?? λ³??  ?
void Context::Reshape(int width, int height)
{
    m_width = width;
    m_height = height;

    // λ·? ?¬?Έ? ?¬κΈ°λ?? ???° ?¬κΈ°μ?? ??Ό?κ²? ?¬μ‘°μ 
    glViewport(0, 0, m_width, m_height);

    m_framebuffer = Framebuffer::Create(Texture::Create(m_width, m_height, GL_RGBA));
}

// λ§μ°?€ ?? ₯? ?°λ₯? μΉ΄λ©?Ό ?? κ°μ ?Έ?
void Context::MouseMove(double x, double y)
{
    // λ§μ°?€ ?€λ₯? μͺ? ?€κ°? ??¬? Έ ?μ§? ??€λ©?, λ³??λ₯? μ£Όμ?? ???€
    if(!m_cameraControl) return;

    // ??¬ λ§μ°?€ μ»€μ ?μΉ? => ?€?¬λ¦? κ³΅κ°??
    auto pos = glm::vec2((float)x, (float)y);

    // λ§μ°?€? ???μ§μ κ³μ°
    // Context λ©€λ² λ³??λ₯? ?΅?΄?, ?΄?  λ§μ°?€ μ»€μ? ?μΉλ?? κΈ°μ΅?΄ ???€
    auto deltaPos = pos - m_prevMousePos;

    // ?λ©΄μ? x μΆ? ?΄? => μΉ΄λ©?Ό? y μΆ? κΈ°μ?? ?? 
    m_cameraYaw -= deltaPos.x * m_cameraRotSpeed;
    // ?λ©΄μ? y μΆ? ?΄? => μΉ΄λ©?Ό? x μΆ? κΈ°μ?? ?? 
    m_cameraPitch -= deltaPos.y * m_cameraRotSpeed;

    // μΉ΄λ©?Ό? ??  κ°λλ₯? μ‘°μ 
    if (m_cameraYaw < 0.0f)   m_cameraYaw += 360.0f;
    if (m_cameraYaw > 360.0f) m_cameraYaw -= 360.0f;

    if (m_cameraPitch > 89.0f)  m_cameraPitch = 89.0f;
    if (m_cameraPitch < -89.0f) m_cameraPitch = -89.0f;

    // ?μΉ? κ°±μ 
    m_prevMousePos = pos;
}

// λ§μ°?€ ?΄λ¦?? μ²λ¦¬
void Context::MouseButton(int button, int action, double x, double y)
{
    if (button == GLFW_MOUSE_BUTTON_RIGHT)  // ?°μΈ? ?€ ?΄λ¦?
    {
        if (action == GLFW_PRESS)
        {
            // λ§μ°?€ μ‘°μ ?? ?? ? ??¬ λ§μ°?€ μ»€μ ?μΉ? ????₯
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
    ?λ©΄μ κ·Έλ¦¬? ??? μ§ν?? ?¨?
    λ§? ?? ?λ§λ€ ?ΈμΆλ?€
    Draw Call ? ?ΈμΆ?
 */
void Context::Render()
{
 
    /*
        ?½???? μΆλ ₯?? Frame Buffer ??

        Color Buffer μ΄κΈ°? <- ?€? ?΄? Clear Color λ‘? μ΄κΈ°?
        Depth Buffer μ΄κΈ°?
     */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    // κΉμ΄ ??€?Έ, κΉμ΄ μ²΄ν¬λ₯? μ§ν?κ² λ€
    glEnable(GL_DEPTH_TEST);
    //glDisable(GL_DEPTH_TEST); // => UI λ₯? κ·Έλ¦΄ ? ?¬?©

    /*
        λ§μ°?€ ?? ₯ => μΉ΄λ©?Ό ?? κ°μ΄ ?¬?Όμ§?
            μ§?κΈκΉμ§? ?? ₯λ°μ?? Camera Parameter λ₯? λ°ν?Όλ‘?
            Camera Front λ²‘ν°λ₯? ?€? κ²μ°
     */
    m_cameraFront =
        // y μΆ? ?? 
        glm::rotate(glm::mat4(1.0f),
            glm::radians(m_cameraYaw), glm::vec3(0.0f, 1.0f, 0.0f)) *
        // x μΆ? ?? 
        glm::rotate(glm::mat4(1.0f),
            glm::radians(m_cameraPitch), glm::vec3(1.0f, 0.0f, 0.0f)) *
        // κΈ°λ³Έ? ?Όλ‘λ (0, 0, -1) ? λ°λΌλ³΄λ λ²‘ν°????€ -
        glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);

    /*
        ?¬? λ³?? ?€? κ΅¬νκ³?
     */
    auto projection = glm::perspective
    (
        glm::radians(45.0f),
        (float)m_width / (float)m_height,
        0.01f, 100.0f
    );

    // lookAt ?¨?λ₯? ?΄?©?΄?, λ·? λ³??? κ΅¬ν?€
    auto view = glm::lookAt
    (
        m_cameraPos,                 // ??¬ μΉ΄λ©?Ό ?μΉ?
        m_cameraPos + m_cameraFront, // ?μΉμ λ²‘ν°λ₯? ??΄?, λ°λΌλ³΄λ μ’νλ₯? κ΅¬ν?€
        m_cameraUp                   // μΉ΄λ©?Ό? UP ?μΉ? μ’ν
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