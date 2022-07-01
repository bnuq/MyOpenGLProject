#include "context.h"


#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <spdlog/spdlog.h>

#include <glad/glad.h>
#include <glfw/glfw3.h>



// Ã¢ Å©±â°¡ º¯ÇßÀ» ¶§
void OnFramebufferSizeChange(GLFWwindow* window, int width, int height)
{
    SPDLOG_INFO("framebuffer size changed: ({} x {})", width, height);

    auto context = reinterpret_cast<Context*>(glfwGetWindowUserPointer(window));
    context->Reshape(width, height);
}

// Å° ÀÔ·Â ¹ß»ı ½Ã
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

    // SPACE Å°°¡ Ã³À½ ÇÑ¹ø ´­·ÈÀ» ¶§ => Á¡ÇÁ ¹ßµ¿
    if(key == GLFW_KEY_SPACE && action == GLFW_PRESS)
    {
        SPDLOG_INFO("Space Key Pressed");
        auto context = reinterpret_cast<Context*>(glfwGetWindowUserPointer(window));
        context->MakeJump();
    }

    // Left Shift ÀÌ ´­·ÈÀ» ¶§ => ´ë½¬ ¹ßµ¿
    if(key == GLFW_KEY_LEFT_SHIFT && action == GLFW_PRESS)
    {
        SPDLOG_INFO("Left Shift Key Pressed");
        auto context = reinterpret_cast<Context*>(glfwGetWindowUserPointer(window));
        context->MakeDash();
    }


    // ESC => ÇÁ·Î±×·¥ Á¾·á
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
}

// ¸¶¿ì½º À§Ä¡ º¯°æ ½Ã
void OnCursorPos(GLFWwindow* window, double x, double y)
{
    auto context = (Context*)glfwGetWindowUserPointer(window);
    context->MouseMove(x, y);
}

// ¸¶¿ì½º ¹öÆ°
void OnMouseButton(GLFWwindow* window, int button, int action, int modifier)
{
    ImGui_ImplGlfw_MouseButtonCallback(window, button, action, modifier);

    auto context = (Context*)glfwGetWindowUserPointer(window);
    double x, y;
    // ¸¶¿ì½º Ä¿¼­ ÇöÀç À§Ä¡
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


    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);  //3.3 version ÀÌ¿´´Âµ¥¿ä
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
    // ¿©±â¼­ºÎÅÍ gl ÇÔ¼ö¸¦ »ç¿ëÇÒ ¼ö ÀÖ´Ù

    auto glVersion = glGetString(GL_VERSION);
    SPDLOG_INFO("OpenGL context version: {}", glVersion);


    // imgui initialization
    // OpenGL context ë¥? ì´ˆê¸°?™” ?•œ ?›„, ImGUI ?„ ì´ˆê¸°?™”ë¥? ?•œ?‹¤
    auto imguiContext = ImGui::CreateContext();     // imgui context
    ImGui::SetCurrentContext(imguiContext);         // current ë¡? ?„¤? •

    ImGui_ImplGlfw_InitForOpenGL(window, false);    // callback ?— ????•œ ?„¤? •??? false => ì§ì ‘?•˜ê² ë‹¤
    ImGui_ImplOpenGL3_Init();
    ImGui_ImplOpenGL3_CreateFontsTexture();
    ImGui_ImplOpenGL3_CreateDeviceObjects();






    // ?‚´ê°? ë§Œë“œ?Š”, ê·¸ë¦¬ê¸? ?œ„?•œ Context
    // ?™”ë©´ì„ ê·¸ë¦¬?Š” ?‘?—…?„ ì§„í–‰?•˜?Š” Context obj ?ƒ?„±
    auto context = Context::Create();
    if (!context) {
        SPDLOG_ERROR("failed to create context");
        glfwTerminate();
        return -1;
    }

    // ?œ ??? ?¬?¸?„° ?‚¬?š©
    // ?Š¹? • ?œˆ?„?š°?— ë¶™ì—ˆ?Š” ?¬?¸?„°ë¥? ê°?? ¸?‹¤ ?“¸ ?ˆ˜ ?ˆ?‹¤
    glfwSetWindowUserPointer(window, context.get());



    // ë·? ?¬?Š¸ ?ƒ?„±?˜ ê²½ìš°, ì²˜ìŒ ?œˆ?„?š° ?ƒ?„± ?•Œ ?‚¬?´ì¦ˆê?? ë³??•˜?Š” => ?´ë²¤íŠ¸ê°? ë°œìƒ?•˜ì§? ?•Š?œ¼ë¯?ë¡?
    // ì²? ?˜¸ì¶œì?? ì§ì ‘ ?•œ?‹¤
    OnFramebufferSizeChange(window, WINDOW_WIDTH, WINDOW_HEIGHT);


    /* GLFW CallBack ?„ ?œˆ?„?š°?— ?“±ë¡? */

    // ?”„? ˆ?„ ë²„í¼ë¥? ?¬?„¤? •?•˜?Š” GLFW ?˜ ì½œë°±?„ ?„¤? • => ?•?œ¼ë¡? GL ?´ ?•Œ?•„?„œ ?˜¸ì¶œí•˜ê²? ?œ?‹¤
    glfwSetFramebufferSizeCallback(window, OnFramebufferSizeChange);

    // ?‚¤ ?…? ¥?„ ì²˜ë¦¬?•˜?Š” GLFW ì½œë°± ?„¤? •
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
            // Å° ÀÔ·ÂÀ» ¸ÕÀú ¹ŞÀº ´ÙÀ½¿¡
            glfwPollEvents();

            // IMGUI ¼³Á¤
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            // xz Å° ÀÔ·Â, È®ÀÎ, ÀÌµ¿ ¿©ºÎ È®ÀÎ
            context->ProcessInput(window);

            // ÀÌÈÄ¿¡ ·»´õ¸µ ÁøÇà
            context->Render();

            // IMGUI ±×¸®±â
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            // Double Buffering, Frame Buffer Swap
            glfwSwapBuffers(window);

            // ·»´õ¸µÀ» ÇÏ°í ³ª¼­, prevTime °»½Å
            prevTime = curTime;
        }
    }



    // ?œˆ?„?š°ê°? ì¢…ë£Œê°? ?˜ë©?, ?´? œê¹Œì?? ?‚¬?š©?–ˆ?˜ GL ë¦¬ì†Œ?Š¤ë¥? ëª¨ë‘ ? •ë¦¬í•´?•¼ ?•œ?‹¤
    // main ?•¨?ˆ˜ ì¢…ë£Œ ? „?—, ë¨¼ì?? GL ë¦¬ì†Œ?Š¤ ? •ë¦¬ë?? ëª…ì‹œ?•œ?‹¤
    //context.reset();  => Context obj ë¥? ê°?ë¦¬í‚¤?˜ ?Š¤ë§ˆíŠ¸ ?¬?¸?„° ?´?š© => ?• ?‹¹?•œ ë¦¬ì†Œ?Š¤ ? •ë¦?
    context = nullptr; // nullptr ????…?•˜ë©? ?Š¤ë§ˆíŠ¸ ?¬?¸?„°ê°? ?•Œ?•„?„œ ? •ë¦¬í•œ?‹¤



    // ì¢…ë£Œ ? „, ImGui ?— ?• ?‹¹?•œ ë¦¬ì†Œ?Š¤ë¥? ë°˜í™˜
    ImGui_ImplOpenGL3_DestroyFontsTexture();
    ImGui_ImplOpenGL3_DestroyDeviceObjects();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext(imguiContext);



    // glfw ë¦¬ì†Œ?Š¤ ? •ë¦?
    glfwTerminate();

    return 0;
}

/* 
    OpenGL Function ì¢…ë¥˜
    
    1. State Setting function
        OpenGL Context ?— ê°’ì„ ????¥?•˜?Š” ?•¨?ˆ˜
        glViewport(+)
        glClearColor(+)
        
    2. State Using function
        Context ?— ????¥?œ State ë¥? ?´?š©?•˜?Š” ?•¨?ˆ˜
        ?‹¤? œë¡? ê·¸ë¦¬ê±°ë‚˜ ?‚˜????‚¸?‹¤
        glClear(+)
*/