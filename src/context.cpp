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

// ?ôîÎ©¥ÏùÑ Í∑∏Î¶¨?äî ?ç∞ ?ïÑ?öî?ïú Î™®Îì† ?öî?Üå?ì§?ùÑ Ï¥àÍ∏∞?ôî
bool Context::Init()
{
    // Í∏∞Î≥∏ Î∞∞Í≤Ω?Éâ?ù∏ Color Buffer Ï¥àÍ∏∞?ôîfgfg§©§∑§©§∑§©§∑§§§©
    glClearColor(0.1f, 0.2f, 0.3f, 0.0f);

    // Programs
    m_program = Program::Create("./shader/lighting.vs", "./shader/lighting.fs");
    if(!m_program)
        return false;


    // Player Mesh, Material ?Ñ§?†ï
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







// ?Ç§Î≥¥ÎìúÎ•? ?Üµ?ï¥ ?ûÖ?†•?êú ?Ç§ Ï≤òÎ¶¨
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

// ?úà?èÑ?ö∞ ?Å¨Í∏∞Í?? Î≥??ï† ?ïå
void Context::Reshape(int width, int height)
{
    m_width = width;
    m_height = height;

    // Î∑? ?è¨?ä∏?ùò ?Å¨Í∏∞Î?? ?úà?èÑ?ö∞ ?Å¨Í∏∞Ï?? ?èô?ùº?ïòÍ≤? ?û¨Ï°∞Ï†ï
    glViewport(0, 0, m_width, m_height);

    m_framebuffer = Framebuffer::Create(Texture::Create(m_width, m_height, GL_RGBA));
}

// ÎßàÏö∞?ä§ ?ûÖ?†•?óê ?î∞Î•? Ïπ¥Î©î?ùº ?öå?†ÑÍ∞ÅÏùÑ ?Ñ∏?åÖ
void Context::MouseMove(double x, double y)
{
    // ÎßàÏö∞?ä§ ?ò§Î•? Ï™? ?Ç§Í∞? ?àå?ü¨?†∏ ?ûàÏß? ?ïä?ã§Î©?, Î≥??ôîÎ•? Ï£ºÏ?? ?ïä?äî?ã§
    if(!m_cameraControl) return;

    // ?òÑ?û¨ ÎßàÏö∞?ä§ Ïª§ÏÑú ?úÑÏπ? => ?ä§?Å¨Î¶? Í≥µÍ∞Ñ?óê?Ñú
    auto pos = glm::vec2((float)x, (float)y);

    // ÎßàÏö∞?ä§?ùò ???ÏßÅÏûÑ Í≥ÑÏÇ∞
    // Context Î©§Î≤Ñ Î≥??àòÎ•? ?Üµ?ï¥?Ñú, ?ù¥?†Ñ ÎßàÏö∞?ä§ Ïª§ÏÑú?ùò ?úÑÏπòÎ?? Í∏∞Ïñµ?ï¥ ?Üì?äî?ã§
    auto deltaPos = pos - m_prevMousePos;

    // ?ôîÎ©¥Ïóê?Ñú x Ï∂? ?ù¥?èô => Ïπ¥Î©î?ùº?äî y Ï∂? Í∏∞Ï?? ?öå?†Ñ
    m_cameraYaw -= deltaPos.x * m_cameraRotSpeed;
    // ?ôîÎ©¥Ïóê?Ñú y Ï∂? ?ù¥?èô => Ïπ¥Î©î?ùº?äî x Ï∂? Í∏∞Ï?? ?öå?†Ñ
    m_cameraPitch -= deltaPos.y * m_cameraRotSpeed;

    // Ïπ¥Î©î?ùº?ùò ?öå?†Ñ Í∞ÅÎèÑÎ•? Ï°∞Ï†ï
    if (m_cameraYaw < 0.0f)   m_cameraYaw += 360.0f;
    if (m_cameraYaw > 360.0f) m_cameraYaw -= 360.0f;

    if (m_cameraPitch > 89.0f)  m_cameraPitch = 89.0f;
    if (m_cameraPitch < -89.0f) m_cameraPitch = -89.0f;

    // ?úÑÏπ? Í∞±Ïã†
    m_prevMousePos = pos;
}

// ÎßàÏö∞?ä§ ?Å¥Î¶??ùÑ Ï≤òÎ¶¨
void Context::MouseButton(int button, int action, double x, double y)
{
    if (button == GLFW_MOUSE_BUTTON_RIGHT)  // ?ö∞Ï∏? ?Ç§ ?Å¥Î¶?
    {
        if (action == GLFW_PRESS)
        {
            // ÎßàÏö∞?ä§ Ï°∞Ïûë ?ãú?ûë ?ãú?†ê?óê ?òÑ?û¨ ÎßàÏö∞?ä§ Ïª§ÏÑú ?úÑÏπ? ????û•
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
    ?ôîÎ©¥Ïóê Í∑∏Î¶¨?äî ?ûë?óÖ?ùÑ ÏßÑÌñâ?ïò?äî ?ï®?àò
    Îß? ?îÑ?†à?ûÑÎßàÎã§ ?ò∏Ï∂úÎêú?ã§
    Draw Call ?ùÑ ?ò∏Ï∂?
 */
void Context::Render()
{
 
    /*
        ?îΩ????ùÑ Ï∂úÎ†•?ïò?äî Frame Buffer ?óê?Ñú

        Color Buffer Ï¥àÍ∏∞?ôî <- ?Ñ§?†ï?ï¥?ëî Clear Color Î°? Ï¥àÍ∏∞?ôî
        Depth Buffer Ï¥àÍ∏∞?ôî
     */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    // ÍπäÏù¥ ?Öå?ä§?ä∏, ÍπäÏù¥ Ï≤¥ÌÅ¨Î•? ÏßÑÌñâ?ïòÍ≤†Îã§
    glEnable(GL_DEPTH_TEST);
    //glDisable(GL_DEPTH_TEST); // => UI Î•? Í∑∏Î¶¥ ?ïå ?Ç¨?ö©

    /*
        ÎßàÏö∞?ä§ ?ûÖ?†• => Ïπ¥Î©î?ùº ?öå?†ÑÍ∞ÅÏù¥ ?ã¨?ùºÏß?
            Ïß?Í∏àÍπåÏß? ?ûÖ?†•Î∞õÏ?? Camera Parameter Î•? Î∞îÌÉï?úºÎ°?
            Camera Front Î≤°ÌÑ∞Î•? ?ã§?ãú Í≤åÏÇ∞
     */
    m_cameraFront =
        // y Ï∂? ?öå?†Ñ
        glm::rotate(glm::mat4(1.0f),
            glm::radians(m_cameraYaw), glm::vec3(0.0f, 1.0f, 0.0f)) *
        // x Ï∂? ?öå?†Ñ
        glm::rotate(glm::mat4(1.0f),
            glm::radians(m_cameraPitch), glm::vec3(1.0f, 0.0f, 0.0f)) *
        // Í∏∞Î≥∏?†Å?úºÎ°úÎäî (0, 0, -1) ?ùÑ Î∞îÎùºÎ≥¥Îäî Î≤°ÌÑ∞????ã§ -
        glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);

    /*
        ?à¨?òÅ Î≥??ôò ?ã§?ãú Íµ¨ÌïòÍ≥?
     */
    auto projection = glm::perspective
    (
        glm::radians(45.0f),
        (float)m_width / (float)m_height,
        0.01f, 100.0f
    );

    // lookAt ?ï®?àòÎ•? ?ù¥?ö©?ï¥?Ñú, Î∑? Î≥??ôò?ùÑ Íµ¨Ìïú?ã§
    auto view = glm::lookAt
    (
        m_cameraPos,                 // ?òÑ?û¨ Ïπ¥Î©î?ùº ?úÑÏπ?
        m_cameraPos + m_cameraFront, // ?úÑÏπòÏóê Î≤°ÌÑ∞Î•? ?çî?ï¥?Ñú, Î∞îÎùºÎ≥¥Îäî Ï¢åÌëúÎ•? Íµ¨Ìïú?ã§
        m_cameraUp                   // Ïπ¥Î©î?ùº?ùò UP ?úÑÏπ? Ï¢åÌëú
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