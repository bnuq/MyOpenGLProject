
/* 
    ?λ©΄μ κ·Έλ €?΄? ?? κ΄?? ¨? κ²λ€?
        GL ?¨?, GL κ°μ²΄
    main ?¨?? μ½λλ₯? ??¬?΄μ§? ?κ³?
    Context Class λ₯? ?΄?©?΄? κ΄?λ¦¬ν?€
*/
#include "context.h"

// imgui ?€???Ό μΆκ??
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>



// ?ΈλΆ? ?Ό?΄λΈλ¬λ¦?
#include <spdlog/spdlog.h>
// glad ?Ό?΄λΈλ¬λ¦?, GLFW ?΄? ? μΆκ??
#include <glad/glad.h>
#include <GLFW/glfw3.h>



/* GLFW κ°? CallBack ?Όλ‘? ?¬?©?  ?¨??€? ? ? */
// ???° ?¬?΄μ¦? λ³?κ²? ?, λ·? ?¬?Έ ?¬κΈ°λ?? ?¬? ??? ?¨?
void OnFramebufferSizeChange(GLFWwindow* window, int width, int height) {
    SPDLOG_INFO("framebuffer size changed: ({} x {})", width, height);
    /* 
        glViewport

        OpenGL ?΄ κ·Έλ¦Ό? κ·Έλ¦΄ ?λ©΄μ ?μΉ? λ°? ?¬κΈ? ?€? 
        λ§λ€?΄μ§? ???° ?΄?? ?€? 
     */
    
    //glViewport(0, 0, width, height);

    // ? ??? ?¬?Έ?°λ₯? ?΅?΄?, ???°? ?? μ»¨ν?€?Έ ? ? κ·Όν?€
    auto context = reinterpret_cast<Context*>(glfwGetWindowUserPointer(window));
    // λ·? ?¬?Έ ?¬?΄μ¦λ?? ?¬μ‘°μ ?λ©΄μ, ?λ‘μ ?? μ’ν‘λΉλ ?¬μ‘°μ 
    context->Reshape(width, height);
}
// ?€λ³΄λ ?? ₯? μ²λ¦¬?? ?¨?
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
    // ? ??? ?¬?Έ?° ?¬?© => ???°? context ? ? κ·?
    auto context = (Context*)glfwGetWindowUserPointer(window);
    context->MouseMove(x, y);
}

void OnMouseButton(GLFWwindow* window, int button, int action, int modifier) {

    ImGui_ImplGlfw_MouseButtonCallback(window, button, action, modifier);

    auto context = (Context*)glfwGetWindowUserPointer(window);
    double x, y;

    // μ»€μ? ?μΉ? κ°?? Έ?΄
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
        ?λ‘?κ²? speed log λ₯? ?΄?©
        ?κ°? μΆλ ₯
        ?Ό?Έ λ²νΈ μΆλ ₯
    */    
    SPDLOG_INFO("Start Program");


    SPDLOG_INFO("Initialize glfw");
    /*
        glfwInit() => glfw ?Ό?΄λΈλ¬λ¦? ?μ²΄λ?? μ΄κΈ°?

        λ―Έλ¦¬ glfw ?Ό?΄λΈλ¬λ¦¬μ ? ???΄ ?? λ³???€? λ§λ€κ³?, μ΄κΈ°? ??€
        glfw ?Ό?΄λΈλ¬λ¦¬κ?? ?¬?©?? λ¦¬μ?€κ°? μ‘΄μ¬?κΈ? ?λ¬Έμ, ?΄?€? ? ?Ή?κ³? ??±?? ?¨κ³?
            λ¦¬μ?€?κΉ?, glfw functions, glfw variables λ₯? ?¬?©?κΈ? ??΄?
            λ°λ? ??

        λ¦¬μ?€λ₯? ? ?Ή?κΈ? ?λ¬Έμ, λ°λ? μ’λ£ ?  λ¦¬μ?€λ₯? ?΄? ?? ??? ????€
            glfwTerminate()
    */
    if (!glfwInit()) {    // glfw ?Ό?΄λΈλ¬λ¦? μ΄κΈ°?, ?€?¨?λ©? ??¬ μΆλ ₯? μ’λ£
        const char* description = nullptr;
        glfwGetError(&description);
        SPDLOG_ERROR("failed to initialize glfw: {}", description);
        return -1;
    }




    /* 
        OpenGL μ΄κΈ°?
            1. GLFW ???° ??± ?  ?¬λ§ν? opengl λ²μ  μΆκ??
            ?¬?©?? €? λ²μ  ??Έ
     */
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);  //3.3 version
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    // glfw ???° ??±
    SPDLOG_INFO("Create glfw window");
    auto window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_NAME, nullptr, nullptr);
    if (!window) {
        SPDLOG_ERROR("failed to create glfw window");
        glfwTerminate();
        return -1;
    }


    /* 
        OpenGL μ΄κΈ°?
            2. ???° ??± ?, ?΄?Ή ???°?? OpenGL Context ?Ό? κ²? ??±??€
            λ§λ€?΄μ§? ???°? Context λ₯? ?¬?©?κ² λ€, ?΄?Ή ???°?? κ·Έλ¦¬κ² λ€ ?Ό? κ²μ ?? €?Ό ??€
     */
    glfwMakeContextCurrent(window);


    /* 
        OpenGL μ΄κΈ°?
            3. glad ?Ό?΄λΈλ¬λ¦¬λ?? ?΅?΄? -> κ°? κ·Έλ?½ μΉ΄λ? λ§λ OpenGL ?¨?λ₯? λ‘λ©
            OpenGL Context ??± ?΄? ?€?
            ???°λ₯? ?΅?΄ λ§λ€?΄μ§? OpenGL Context λ₯? ?¬?©??€
     */
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        SPDLOG_ERROR("failed to initialize glad");
        glfwTerminate();
        return -1;
    }
    //------ ?΄?  ?΄ λ°μ?λΆ??°, OpenGL ?¨?λ₯? ?¬?©?  ? ??€ ------//



    auto glVersion = glGetString(GL_VERSION); //??±? OpenGL Context ?€?  λ²μ 
    SPDLOG_INFO("OpenGL context version: {}", glVersion);


    // imgui initialization
    // OpenGL context λ₯? μ΄κΈ°? ? ?, ImGUI ? μ΄κΈ°?λ₯? ??€
    auto imguiContext = ImGui::CreateContext();     // imgui context
    ImGui::SetCurrentContext(imguiContext);         // current λ‘? ?€? 

    ImGui_ImplGlfw_InitForOpenGL(window, false);    // callback ? ???? ?€? ??? false => μ§μ ?κ² λ€
    ImGui_ImplOpenGL3_Init();
    ImGui_ImplOpenGL3_CreateFontsTexture();
    ImGui_ImplOpenGL3_CreateDeviceObjects();






    // ?΄κ°? λ§λ?, κ·Έλ¦¬κΈ? ?? Context
    // ?λ©΄μ κ·Έλ¦¬? ??? μ§ν?? Context obj ??±
    auto context = Context::Create();
    if (!context) {
        SPDLOG_ERROR("failed to create context");
        glfwTerminate();
        return -1;
    }

    // ? ??? ?¬?Έ?° ?¬?©
    // ?Ή?  ???°? λΆμ? ?¬?Έ?°λ₯? κ°?? Έ?€ ?Έ ? ??€
    glfwSetWindowUserPointer(window, context.get());



    // λ·? ?¬?Έ ??±? κ²½μ°, μ²μ ???° ??± ? ?¬?΄μ¦κ?? λ³??? => ?΄λ²€νΈκ°? λ°μ?μ§? ??Όλ―?λ‘?
    // μ²? ?ΈμΆμ?? μ§μ  ??€
    OnFramebufferSizeChange(window, WINDOW_WIDTH, WINDOW_HEIGHT);


    /* GLFW CallBack ? ???°? ?±λ‘? */

    // ?? ? λ²νΌλ₯? ?¬?€? ?? GLFW ? μ½λ°±? ?€?  => ??Όλ‘? GL ?΄ ??? ?ΈμΆνκ²? ??€
    glfwSetFramebufferSizeCallback(window, OnFramebufferSizeChange);

    // ?€ ?? ₯? μ²λ¦¬?? GLFW μ½λ°± ?€? 
    glfwSetKeyCallback(window, OnKeyEvent);
    	
    glfwSetCharCallback(window, OnCharEvent);

    glfwSetCursorPosCallback(window, OnCursorPos);
    	
    glfwSetMouseButtonCallback(window, OnMouseButton);

    glfwSetScrollCallback(window, OnScroll);

    // glfw λ©μΈ λ£¨ν ?€?, ???° close λ²νΌ? ?λ₯΄λ©΄ ? ? μ’λ£
    SPDLOG_INFO("Start main loop");
    while (!glfwWindowShouldClose(window)) {
        
        // window ?? λ°μ?? ?΄λ²€νΈλ₯? ?μ§?
        // ?΄λ²€νΈ λ°μ ??, ???Όλ‘? κ·Έμ λ§λ μ½λ°± ?¨?λ₯? ?ΈμΆν΄μ€??€
        glfwPollEvents();


        // UI λ₯? λ§? ?? ?λ§λ€ κ·Έλ¦°?€
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame(); // μ§?κΈλ???° ? ? ?λ§? ?? ??΄?€

       
        // ???°? ?€?΄?€? ?Έ?? μ²λ¦¬
        context->ProcessInput(window);


        // Context Object ?΄?© => ?λ©΄μ κ·Έλ¦¬? ??? μ§ν??€
        context->Render();


        // ?¬? ? ?λ§? ? ?, UI λ₯? ? ?λ§ν?€
        ImGui::Render();    // κ·Έλ €?Ό ?? ? λ³΄λ?? κ°?? Έ?€κ³?
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData()); // κ·Έλ¦°?€


        /* 
            Double Buffering, Frame Buffer Swap

            Front buffer ??? Back Buffer λ₯? ?°λ£? κ΅¬ν, λ¨Όμ?? back buffer ? κ·Έλ¦° ?€??
            Front Buffer ??? Back Buffer λ₯? λ°κΎΌ?€
         */
        glfwSwapBuffers(window);    // ?? ? λ²νΌ ?€? μ½λ
    }



    // ???°κ°? μ’λ£κ°? ?λ©?, ?΄? κΉμ?? ?¬?©?? GL λ¦¬μ?€λ₯? λͺ¨λ ? λ¦¬ν΄?Ό ??€
    // main ?¨? μ’λ£ ? ?, λ¨Όμ?? GL λ¦¬μ?€ ? λ¦¬λ?? λͺμ??€
    //context.reset();  => Context obj λ₯? κ°?λ¦¬ν€? ?€λ§νΈ ?¬?Έ?° ?΄?© => ? ?Ή? λ¦¬μ?€ ? λ¦?
    context = nullptr; // nullptr ?????λ©? ?€λ§νΈ ?¬?Έ?°κ°? ??? ? λ¦¬ν?€



    // μ’λ£ ? , ImGui ? ? ?Ή? λ¦¬μ?€λ₯? λ°ν
    ImGui_ImplOpenGL3_DestroyFontsTexture();
    ImGui_ImplOpenGL3_DestroyDeviceObjects();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext(imguiContext);



    // glfw λ¦¬μ?€ ? λ¦?
    glfwTerminate();

    return 0;
}

/* 
    OpenGL Function μ’λ₯
    
    1. State Setting function
        OpenGL Context ? κ°μ ????₯?? ?¨?
        glViewport(+)
        glClearColor(+)
        
    2. State Using function
        Context ? ????₯? State λ₯? ?΄?©?? ?¨?
        ?€? λ‘? κ·Έλ¦¬κ±°λ ?????Έ?€
        glClear(+)
*/