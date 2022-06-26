
/* 
    ?™”ë©´ì— ê·¸ë ¤?‚´?Š” ?‘?—… ê´?? ¨?œ ê²ƒë“¤?„
        GL ?•¨?ˆ˜, GL ê°ì²´
    main ?•¨?ˆ˜?— ì½”ë“œë¥? ?“œ?Ÿ¬?‚´ì§? ?•Šê³?
    Context Class ë¥? ?´?š©?•´?„œ ê´?ë¦¬í•œ?‹¤
*/
#include "context.h"

// imgui ?—¤?”?ŒŒ?¼ ì¶”ê??
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>



// ?™¸ë¶? ?¼?´ë¸ŒëŸ¬ë¦?
#include <spdlog/spdlog.h>
// glad ?¼?´ë¸ŒëŸ¬ë¦?, GLFW ?´? „?— ì¶”ê??
#include <glad/glad.h>
#include <GLFW/glfw3.h>



/* GLFW ê°? CallBack ?œ¼ë¡? ?‚¬?š©?•  ?•¨?ˆ˜?“¤?„ ? •?˜ */
// ?œˆ?„?š° ?‚¬?´ì¦? ë³?ê²? ?‹œ, ë·? ?¬?Š¸ ?¬ê¸°ë?? ?¬? •?˜?•˜?Š” ?•¨?ˆ˜
void OnFramebufferSizeChange(GLFWwindow* window, int width, int height) {
    SPDLOG_INFO("framebuffer size changed: ({} x {})", width, height);
    /* 
        glViewport

        OpenGL ?´ ê·¸ë¦¼?„ ê·¸ë¦´ ?™”ë©´ì˜ ?œ„ì¹? ë°? ?¬ê¸? ?„¤? •
        ë§Œë“¤?–´ì§? ?œˆ?„?š° ?‚´?—?„œ ?„¤? •
     */
    
    //glViewport(0, 0, width, height);

    // ?œ ??? ?¬?¸?„°ë¥? ?†µ?•´?„œ, ?œˆ?„?š°?— ?†?•œ ì»¨íƒ?Š¤?Š¸ ?— ? ‘ê·¼í•œ?‹¤
    auto context = reinterpret_cast<Context*>(glfwGetWindowUserPointer(window));
    // ë·? ?¬?Š¸ ?‚¬?´ì¦ˆë?? ?¬ì¡°ì •?•˜ë©´ì„œ, ?”„ë¡œì ?…˜?˜ ì¢…íš¡ë¹„ë„ ?¬ì¡°ì •
    context->Reshape(width, height);
}
// ?‚¤ë³´ë“œ ?…? ¥?„ ì²˜ë¦¬?•˜?Š” ?•¨?ˆ˜
void OnKeyEvent(GLFWwindow* window, int key, int scancode, int action, int mods) {

    ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);

    // SPDLOG_INFO("key: {}, scancode: {}, action: {}, mods: {}{}{}",
    //     key,
    //     scancode,

    //     action == GLFW_PRESS ? "Pressed" :
    //     action == GLFW_RELEASE ? "Released" :
    //     action == GLFW_REPEAT ? "Repeat" : "Unknown",

    //     mods & GLFW_MOD_CONTROL ? "C" : "-",
    //     mods & GLFW_MOD_SHIFT ? "S" : "-",
    //     mods & GLFW_MOD_ALT ? "A" : "-"
    // );

    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
    {
        SPDLOG_INFO("Space Key Pressed");
        auto context = reinterpret_cast<Context*>(glfwGetWindowUserPointer(window));
        context->MakeJump();
    }
    if (key == GLFW_KEY_SPACE && action == GLFW_RELEASE)
    {
        SPDLOG_INFO("Space Key Released");
        //auto context = reinterpret_cast<Context*>(glfwGetWindowUserPointer(window));
        //context->PlayerGround();
    }


    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

void OnCursorPos(GLFWwindow* window, double x, double y) {
    // ?œ ??? ?¬?¸?„° ?‚¬?š© => ?œˆ?„?š°?˜ context ?— ? ‘ê·?
    auto context = (Context*)glfwGetWindowUserPointer(window);
    context->MouseMove(x, y);
}

void OnMouseButton(GLFWwindow* window, int button, int action, int modifier) {

    ImGui_ImplGlfw_MouseButtonCallback(window, button, action, modifier);

    auto context = (Context*)glfwGetWindowUserPointer(window);
    double x, y;

    // ì»¤ì„œ?˜ ?œ„ì¹? ê°?? ¸?˜´
    glfwGetCursorPos(window, &x, &y);
    context->MouseButton(button, action, x, y);
}
void OnCharEvent(GLFWwindow* window, unsigned int ch) {
    ImGui_ImplGlfw_CharCallback(window, ch);
}

void OnScroll(GLFWwindow* window, double xoffset, double yoffset) {
    ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
}


int main(int argc, const char** argv) {
    /*
        ?ƒˆë¡?ê²? speed log ë¥? ?´?š©
        ?‹œê°? ì¶œë ¥
        ?¼?¸ ë²ˆí˜¸ ì¶œë ¥
    */    
    SPDLOG_INFO("Start Program");


    SPDLOG_INFO("Initialize glfw");
    /*
        glfwInit() => glfw ?¼?´ë¸ŒëŸ¬ë¦? ?ì²´ë?? ì´ˆê¸°?™”

        ë¯¸ë¦¬ glfw ?¼?´ë¸ŒëŸ¬ë¦¬ì— ? •?˜?˜?–´ ?ˆ?Š” ë³??ˆ˜?“¤?„ ë§Œë“¤ê³?, ì´ˆê¸°?™” ?•œ?‹¤
        glfw ?¼?´ë¸ŒëŸ¬ë¦¬ê?? ?‚¬?š©?•˜?Š” ë¦¬ì†Œ?Š¤ê°? ì¡´ì¬?•˜ê¸? ?•Œë¬¸ì—, ?´?“¤?„ ?• ?‹¹?•˜ê³? ?ƒ?„±?•˜?Š” ?‹¨ê³?
            ë¦¬ì†Œ?Š¤?‹ˆê¹?, glfw functions, glfw variables ë¥? ?‚¬?š©?•˜ê¸? ?œ„?•´?„œ
            ë°˜ë“œ?‹œ ?•„?š”

        ë¦¬ì†Œ?Š¤ë¥? ?• ?‹¹?•˜ê¸? ?•Œë¬¸ì—, ë°˜ë“œ?‹œ ì¢…ë£Œ ? „ ë¦¬ì†Œ?Š¤ë¥? ?•´? œ?•˜?Š” ?‘?—…?„ ?•„?š”?•˜?‹¤
            glfwTerminate()
    */
    if (!glfwInit()) {    // glfw ?¼?´ë¸ŒëŸ¬ë¦? ì´ˆê¸°?™”, ?‹¤?Œ¨?•˜ë©? ?—?Ÿ¬ ì¶œë ¥?›„ ì¢…ë£Œ
        const char* description = nullptr;
        glfwGetError(&description);
        SPDLOG_ERROR("failed to initialize glfw: {}", description);
        return -1;
    }




    /* 
        OpenGL ì´ˆê¸°?™”
            1. GLFW ?œˆ?„?š° ?ƒ?„± ? „ ?¬ë§í•˜?Š” opengl ë²„ì „ ì¶”ê??
            ?‚¬?š©?•˜? ¤?Š” ë²„ì „ ?Œ?Š¸
     */
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);  //3.3 version
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    // glfw ?œˆ?„?š° ?ƒ?„±
    SPDLOG_INFO("Create glfw window");
    auto window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_NAME, nullptr, nullptr);
    if (!window) {
        SPDLOG_ERROR("failed to create glfw window");
        glfwTerminate();
        return -1;
    }


    /* 
        OpenGL ì´ˆê¸°?™”
            2. ?œˆ?„?š° ?ƒ?„± ?›„, ?•´?‹¹ ?œˆ?„?š°?—?Š” OpenGL Context ?¼?Š” ê²? ?ƒ?„±?œ?‹¤
            ë§Œë“¤?–´ì§? ?œˆ?„?š°?˜ Context ë¥? ?‚¬?š©?•˜ê² ë‹¤, ?•´?‹¹ ?œˆ?„?š°?—?„œ ê·¸ë¦¬ê² ë‹¤ ?¼?Š” ê²ƒì„ ?•Œ? ¤?•¼ ?•œ?‹¤
     */
    glfwMakeContextCurrent(window);


    /* 
        OpenGL ì´ˆê¸°?™”
            3. glad ?¼?´ë¸ŒëŸ¬ë¦¬ë?? ?†µ?•´?„œ -> ê°? ê·¸ë˜?”½ ì¹´ë“œ?— ë§ëŠ” OpenGL ?•¨?ˆ˜ë¥? ë¡œë”©
            OpenGL Context ?ƒ?„± ?´?›„ ?‹¤?–‰
            ?œˆ?„?š°ë¥? ?†µ?•´ ë§Œë“¤?–´ì§? OpenGL Context ë¥? ?‚¬?š©?•œ?‹¤
     */
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        SPDLOG_ERROR("failed to initialize glad");
        glfwTerminate();
        return -1;
    }
    //------ ?´? œ ?´ ë°‘ì—?„œë¶??„°, OpenGL ?•¨?ˆ˜ë¥? ?‚¬?š©?•  ?ˆ˜ ?ˆ?‹¤ ------//



    auto glVersion = glGetString(GL_VERSION); //?ƒ?„±?œ OpenGL Context ?‹¤? œ ë²„ì „
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

    // glfw ë©”ì¸ ë£¨í”„ ?‹¤?–‰, ?œˆ?„?š° close ë²„íŠ¼?„ ?ˆ„ë¥´ë©´ ? •?ƒ ì¢…ë£Œ
    SPDLOG_INFO("Start main loop");
    while (!glfwWindowShouldClose(window)) {
        
        // window ?—?„œ ë°œìƒ?•˜?Š” ?´ë²¤íŠ¸ë¥? ?ˆ˜ì§?
        // ?´ë²¤íŠ¸ ë°œìƒ ?‹œ?—, ??™?œ¼ë¡? ê·¸ì— ë§ëŠ” ì½œë°± ?•¨?ˆ˜ë¥? ?˜¸ì¶œí•´ì¤??‹¤
        glfwPollEvents();


        // UI ë¥? ë§? ?”„? ˆ?„ë§ˆë‹¤ ê·¸ë¦°?‹¤
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame(); // ì§?ê¸ˆë???„° ?ƒˆ ? Œ?”ë§? ?”„? ˆ?„?´?‹¤

       
        // ?œˆ?„?š°?— ?“¤?–´?˜¤?Š” ?¸?’‹?„ ì²˜ë¦¬
        context->ProcessInput(window);


        // Context Object ?´?š© => ?™”ë©´ì— ê·¸ë¦¬?Š” ?‘?—…?„ ì§„í–‰?•œ?‹¤
        context->Render();


        // ?”¬?„ ? Œ?”ë§? ?•œ ?›„, UI ë¥? ? Œ?”ë§í•œ?‹¤
        ImGui::Render();    // ê·¸ë ¤?•¼ ?•˜?Š” ? •ë³´ë?? ê°?? ¸?˜¤ê³?
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData()); // ê·¸ë¦°?‹¤


        /* 
            Double Buffering, Frame Buffer Swap

            Front buffer ??? Back Buffer ë¥? ?”°ë£? êµ¬í˜„, ë¨¼ì?? back buffer ?— ê·¸ë¦° ?‹¤?Œ?—
            Front Buffer ??? Back Buffer ë¥? ë°”ê¾¼?‹¤
         */
        glfwSwapBuffers(window);    // ?”„? ˆ?„ ë²„í¼ ?Š¤?™‘ ì½”ë“œ
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