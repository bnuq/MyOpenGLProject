#include "context.h"


#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <spdlog/spdlog.h>

#include <glad/glad.h>
#include <glfw/glfw3.h>



// √¢ ≈©±‚∞° ∫Ø«ﬂ¿ª ∂ß
void OnFramebufferSizeChange(GLFWwindow* window, int width, int height)
{
    SPDLOG_INFO("framebuffer size changed: ({} x {})", width, height);

    auto context = reinterpret_cast<Context*>(glfwGetWindowUserPointer(window));
    context->Reshape(width, height);
}

// ≈∞ ¿‘∑¬ πﬂª˝ Ω√
void OnKeyEvent(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);

    /*
        SPDLOG_INFO
        (   
            "key: {}, scancode: {}, action: {}, mods: {}{}{}",
            key,
            scancode,

            action == GLFW_PRESS ? "Pressed" :
            action == GLFW_RELEASE ? "Released" :
            action == GLFW_REPEAT ? "Repeat" : "Unknown",

            mods & GLFW_MOD_CONTROL ? "C" : "-",
            mods & GLFW_MOD_SHIFT ? "S" : "-",
            mods & GLFW_MOD_ALT ? "A" : "-"
        );
    */

    // SPACE ≈∞∞° √≥¿Ω «—π¯ ¥≠∑»¿ª ∂ß
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
    {
        SPDLOG_INFO("Space Key Pressed");
        auto context = reinterpret_cast<Context*>(glfwGetWindowUserPointer(window));
        context->MakeJump();
    }
    if (key == GLFW_KEY_SPACE && action == GLFW_RELEASE)
    {
        SPDLOG_INFO("Space Key Released");
    }

    // ESC => «¡∑Œ±◊∑• ¡æ∑·
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
}

// ∏∂øÏΩ∫ ¿ßƒ° ∫Ø∞Ê Ω√
void OnCursorPos(GLFWwindow* window, double x, double y)
{
    auto context = (Context*)glfwGetWindowUserPointer(window);
    context->MouseMove(x, y);
}

// ∏∂øÏΩ∫ πˆ∆∞
void OnMouseButton(GLFWwindow* window, int button, int action, int modifier)
{
    ImGui_ImplGlfw_MouseButtonCallback(window, button, action, modifier);

    auto context = (Context*)glfwGetWindowUserPointer(window);
    double x, y;
    // ∏∂øÏΩ∫ ƒøº≠ «ˆ¿Á ¿ßƒ°
    glfwGetCursorPos(window, &x, &y);
    context->MouseButton(button, action, x, y);
}

void OnCharEvent(GLFWwindow* window, unsigned int ch)
{
    ImGui_ImplGlfw_CharCallback(window, ch);
}
void OnScroll(GLFWwindow* window, double xoffset, double yoffset)
{
    ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
}


int main(int argc, const char** argv)
{
    SPDLOG_INFO("Start Program");

    SPDLOG_INFO("Initialize glfw");
    if (!glfwInit())
    {
        const char* description = nullptr;
        glfwGetError(&description);
        SPDLOG_ERROR("failed to initialize glfw: {}", description);
        return -1;
    }


    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);  //3.3 version ¿Ãø¥¥¬µ•ø‰
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    SPDLOG_INFO("Create glfw window");
    auto window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_NAME, nullptr, nullptr);
    if (!window)
    {
        SPDLOG_ERROR("failed to create glfw window");
        glfwTerminate();
        return -1;
    }


    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        SPDLOG_ERROR("failed to initialize glad");
        glfwTerminate();
        return -1;
    }
    // ø©±‚º≠∫Œ≈Õ gl «‘ºˆ∏¶ ªÁøÎ«“ ºˆ ¿÷¥Ÿ

    auto glVersion = glGetString(GL_VERSION);
    SPDLOG_INFO("OpenGL context version: {}", glVersion);


    // imgui initialization
    // OpenGL context Î•? Ï¥àÍ∏∞?ôî ?ïú ?õÑ, ImGUI ?èÑ Ï¥àÍ∏∞?ôîÎ•? ?ïú?ã§
    auto imguiContext = ImGui::CreateContext();     // imgui context
    ImGui::SetCurrentContext(imguiContext);         // current Î°? ?Ñ§?†ï

    ImGui_ImplGlfw_InitForOpenGL(window, false);    // callback ?óê ????ïú ?Ñ§?†ï??? false => ÏßÅÏ†ë?ïòÍ≤†Îã§
    ImGui_ImplOpenGL3_Init();
    ImGui_ImplOpenGL3_CreateFontsTexture();
    ImGui_ImplOpenGL3_CreateDeviceObjects();






    // ?Ç¥Í∞? ÎßåÎìú?äî, Í∑∏Î¶¨Í∏? ?úÑ?ïú Context
    // ?ôîÎ©¥ÏùÑ Í∑∏Î¶¨?äî ?ûë?óÖ?ùÑ ÏßÑÌñâ?ïò?äî Context obj ?Éù?Ñ±
    auto context = Context::Create();
    if (!context) {
        SPDLOG_ERROR("failed to create context");
        glfwTerminate();
        return -1;
    }

    // ?ú†??? ?è¨?ù∏?Ñ∞ ?Ç¨?ö©
    // ?äπ?†ï ?úà?èÑ?ö∞?óê Î∂ôÏóà?äî ?è¨?ù∏?Ñ∞Î•? Í∞??†∏?ã§ ?ì∏ ?àò ?ûà?ã§
    glfwSetWindowUserPointer(window, context.get());



    // Î∑? ?è¨?ä∏ ?Éù?Ñ±?ùò Í≤ΩÏö∞, Ï≤òÏùå ?úà?èÑ?ö∞ ?Éù?Ñ± ?ïå ?Ç¨?ù¥Ï¶àÍ?? Î≥??ïò?äî => ?ù¥Î≤§Ìä∏Í∞? Î∞úÏÉù?ïòÏß? ?ïä?úºÎØ?Î°?
    // Ï≤? ?ò∏Ï∂úÏ?? ÏßÅÏ†ë ?ïú?ã§
    OnFramebufferSizeChange(window, WINDOW_WIDTH, WINDOW_HEIGHT);


    /* GLFW CallBack ?ùÑ ?úà?èÑ?ö∞?óê ?ì±Î°? */

    // ?îÑ?†à?ûÑ Î≤ÑÌçºÎ•? ?û¨?Ñ§?†ï?ïò?äî GLFW ?ùò ÏΩúÎ∞±?ùÑ ?Ñ§?†ï => ?ïû?úºÎ°? GL ?ù¥ ?ïå?ïÑ?Ñú ?ò∏Ï∂úÌïòÍ≤? ?êú?ã§
    glfwSetFramebufferSizeCallback(window, OnFramebufferSizeChange);

    // ?Ç§ ?ûÖ?†•?ùÑ Ï≤òÎ¶¨?ïò?äî GLFW ÏΩúÎ∞± ?Ñ§?†ï
    glfwSetKeyCallback(window, OnKeyEvent);
    	
    glfwSetCharCallback(window, OnCharEvent);

    glfwSetCursorPosCallback(window, OnCursorPos);
    	
    glfwSetMouseButtonCallback(window, OnMouseButton);

    glfwSetScrollCallback(window, OnScroll);

    
    SPDLOG_INFO("Start main loop");



    double prevTime {}, curTime {};
    prevTime = glfwGetTime();

    while (!glfwWindowShouldClose(window))
    {
        curTime = glfwGetTime();       
        // 60FPS Rendering 
        if(curTime - prevTime >= 0.00166)
        {
            glfwPollEvents();

            // IMGUI º≥¡§
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            context->ProcessInput(window);
            context->Render();

            // IMGUI ±◊∏Æ±‚
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            // Double Buffering, Frame Buffer Swap
            glfwSwapBuffers(window);

            // ∑ª¥ı∏µ¿ª «œ∞Ì ≥™º≠, prevTime ∞ªΩ≈
            prevTime = curTime;
        }
    }



    // ?úà?èÑ?ö∞Í∞? Ï¢ÖÎ£åÍ∞? ?êòÎ©?, ?ù¥?†úÍπåÏ?? ?Ç¨?ö©?ñà?çò GL Î¶¨ÏÜå?ä§Î•? Î™®Îëê ?†ïÎ¶¨Ìï¥?ïº ?ïú?ã§
    // main ?ï®?àò Ï¢ÖÎ£å ?†Ñ?óê, Î®ºÏ?? GL Î¶¨ÏÜå?ä§ ?†ïÎ¶¨Î?? Î™ÖÏãú?ïú?ã§
    //context.reset();  => Context obj Î•? Í∞?Î¶¨ÌÇ§?çò ?ä§ÎßàÌä∏ ?è¨?ù∏?Ñ∞ ?ù¥?ö© => ?ï†?ãπ?ïú Î¶¨ÏÜå?ä§ ?†ïÎ¶?
    context = nullptr; // nullptr ????ûÖ?ïòÎ©? ?ä§ÎßàÌä∏ ?è¨?ù∏?Ñ∞Í∞? ?ïå?ïÑ?Ñú ?†ïÎ¶¨Ìïú?ã§



    // Ï¢ÖÎ£å ?†Ñ, ImGui ?óê ?ï†?ãπ?ïú Î¶¨ÏÜå?ä§Î•? Î∞òÌôò
    ImGui_ImplOpenGL3_DestroyFontsTexture();
    ImGui_ImplOpenGL3_DestroyDeviceObjects();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext(imguiContext);



    // glfw Î¶¨ÏÜå?ä§ ?†ïÎ¶?
    glfwTerminate();

    return 0;
}

/* 
    OpenGL Function Ï¢ÖÎ•ò
    
    1. State Setting function
        OpenGL Context ?óê Í∞íÏùÑ ????û•?ïò?äî ?ï®?àò
        glViewport(+)
        glClearColor(+)
        
    2. State Using function
        Context ?óê ????û•?êú State Î•? ?ù¥?ö©?ïò?äî ?ï®?àò
        ?ã§?†úÎ°? Í∑∏Î¶¨Í±∞ÎÇò ?Çò????Ç∏?ã§
        glClear(+)
*/