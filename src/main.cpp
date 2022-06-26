
/* 
    ?��면에 그려?��?�� ?��?�� �??��?�� 것들?��
        GL ?��?��, GL 객체
    main ?��?��?�� 코드�? ?��?��?���? ?���?
    Context Class �? ?��?��?��?�� �?리한?��
*/
#include "context.h"

// imgui ?��?��?��?�� 추�??
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>



// ?���? ?��?��브러�?
#include <spdlog/spdlog.h>
// glad ?��?��브러�?, GLFW ?��?��?�� 추�??
#include <glad/glad.h>
#include <GLFW/glfw3.h>



/* GLFW �? CallBack ?���? ?��?��?�� ?��?��?��?�� ?��?�� */
// ?��?��?�� ?��?���? �?�? ?��, �? ?��?�� ?��기�?? ?��?��?��?��?�� ?��?��
void OnFramebufferSizeChange(GLFWwindow* window, int width, int height) {
    SPDLOG_INFO("framebuffer size changed: ({} x {})", width, height);
    /* 
        glViewport

        OpenGL ?�� 그림?�� 그릴 ?��면의 ?���? �? ?���? ?��?��
        만들?���? ?��?��?�� ?��?��?�� ?��?��
     */
    
    //glViewport(0, 0, width, height);

    // ?��??? ?��?��?���? ?��?��?��, ?��?��?��?�� ?��?�� 컨택?��?�� ?�� ?��근한?��
    auto context = reinterpret_cast<Context*>(glfwGetWindowUserPointer(window));
    // �? ?��?�� ?��?��즈�?? ?��조정?��면서, ?��로젝?��?�� 종횡비도 ?��조정
    context->Reshape(width, height);
}
// ?��보드 ?��?��?�� 처리?��?�� ?��?��
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
    // ?��??? ?��?��?�� ?��?�� => ?��?��?��?�� context ?�� ?���?
    auto context = (Context*)glfwGetWindowUserPointer(window);
    context->MouseMove(x, y);
}

void OnMouseButton(GLFWwindow* window, int button, int action, int modifier) {

    ImGui_ImplGlfw_MouseButtonCallback(window, button, action, modifier);

    auto context = (Context*)glfwGetWindowUserPointer(window);
    double x, y;

    // 커서?�� ?���? �??��?��
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
        ?���?�? speed log �? ?��?��
        ?���? 출력
        ?��?�� 번호 출력
    */    
    SPDLOG_INFO("Start Program");


    SPDLOG_INFO("Initialize glfw");
    /*
        glfwInit() => glfw ?��?��브러�? ?��체�?? 초기?��

        미리 glfw ?��?��브러리에 ?��?��?��?�� ?��?�� �??��?��?�� 만들�?, 초기?�� ?��?��
        glfw ?��?��브러리�?? ?��?��?��?�� 리소?���? 존재?���? ?��문에, ?��?��?�� ?��?��?���? ?��?��?��?�� ?���?
            리소?��?���?, glfw functions, glfw variables �? ?��?��?���? ?��?��?��
            반드?�� ?��?��

        리소?���? ?��?��?���? ?��문에, 반드?�� 종료 ?�� 리소?���? ?��?��?��?�� ?��?��?�� ?��?��?��?��
            glfwTerminate()
    */
    if (!glfwInit()) {    // glfw ?��?��브러�? 초기?��, ?��?��?���? ?��?�� 출력?�� 종료
        const char* description = nullptr;
        glfwGetError(&description);
        SPDLOG_ERROR("failed to initialize glfw: {}", description);
        return -1;
    }




    /* 
        OpenGL 초기?��
            1. GLFW ?��?��?�� ?��?�� ?�� ?��망하?�� opengl 버전 추�??
            ?��?��?��?��?�� 버전 ?��?��
     */
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);  //3.3 version
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    // glfw ?��?��?�� ?��?��
    SPDLOG_INFO("Create glfw window");
    auto window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_NAME, nullptr, nullptr);
    if (!window) {
        SPDLOG_ERROR("failed to create glfw window");
        glfwTerminate();
        return -1;
    }


    /* 
        OpenGL 초기?��
            2. ?��?��?�� ?��?�� ?��, ?��?�� ?��?��?��?��?�� OpenGL Context ?��?�� �? ?��?��?��?��
            만들?���? ?��?��?��?�� Context �? ?��?��?��겠다, ?��?�� ?��?��?��?��?�� 그리겠다 ?��?�� 것을 ?��?��?�� ?��?��
     */
    glfwMakeContextCurrent(window);


    /* 
        OpenGL 초기?��
            3. glad ?��?��브러리�?? ?��?��?�� -> �? 그래?�� 카드?�� 맞는 OpenGL ?��?���? 로딩
            OpenGL Context ?��?�� ?��?�� ?��?��
            ?��?��?���? ?��?�� 만들?���? OpenGL Context �? ?��?��?��?��
     */
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        SPDLOG_ERROR("failed to initialize glad");
        glfwTerminate();
        return -1;
    }
    //------ ?��?�� ?�� 밑에?���??��, OpenGL ?��?���? ?��?��?�� ?�� ?��?�� ------//



    auto glVersion = glGetString(GL_VERSION); //?��?��?�� OpenGL Context ?��?�� 버전
    SPDLOG_INFO("OpenGL context version: {}", glVersion);


    // imgui initialization
    // OpenGL context �? 초기?�� ?�� ?��, ImGUI ?�� 초기?���? ?��?��
    auto imguiContext = ImGui::CreateContext();     // imgui context
    ImGui::SetCurrentContext(imguiContext);         // current �? ?��?��

    ImGui_ImplGlfw_InitForOpenGL(window, false);    // callback ?�� ????�� ?��?��??? false => 직접?��겠다
    ImGui_ImplOpenGL3_Init();
    ImGui_ImplOpenGL3_CreateFontsTexture();
    ImGui_ImplOpenGL3_CreateDeviceObjects();






    // ?���? 만드?��, 그리�? ?��?�� Context
    // ?��면을 그리?�� ?��?��?�� 진행?��?�� Context obj ?��?��
    auto context = Context::Create();
    if (!context) {
        SPDLOG_ERROR("failed to create context");
        glfwTerminate();
        return -1;
    }

    // ?��??? ?��?��?�� ?��?��
    // ?��?�� ?��?��?��?�� 붙었?�� ?��?��?���? �??��?�� ?�� ?�� ?��?��
    glfwSetWindowUserPointer(window, context.get());



    // �? ?��?�� ?��?��?�� 경우, 처음 ?��?��?�� ?��?�� ?�� ?��?��즈�?? �??��?�� => ?��벤트�? 발생?���? ?��?���?�?
    // �? ?��출�?? 직접 ?��?��
    OnFramebufferSizeChange(window, WINDOW_WIDTH, WINDOW_HEIGHT);


    /* GLFW CallBack ?�� ?��?��?��?�� ?���? */

    // ?��?��?�� 버퍼�? ?��?��?��?��?�� GLFW ?�� 콜백?�� ?��?�� => ?��?���? GL ?�� ?��?��?�� ?��출하�? ?��?��
    glfwSetFramebufferSizeCallback(window, OnFramebufferSizeChange);

    // ?�� ?��?��?�� 처리?��?�� GLFW 콜백 ?��?��
    glfwSetKeyCallback(window, OnKeyEvent);
    	
    glfwSetCharCallback(window, OnCharEvent);

    glfwSetCursorPosCallback(window, OnCursorPos);
    	
    glfwSetMouseButtonCallback(window, OnMouseButton);

    glfwSetScrollCallback(window, OnScroll);

    // glfw 메인 루프 ?��?��, ?��?��?�� close 버튼?�� ?��르면 ?��?�� 종료
    SPDLOG_INFO("Start main loop");
    while (!glfwWindowShouldClose(window)) {
        
        // window ?��?�� 발생?��?�� ?��벤트�? ?���?
        // ?��벤트 발생 ?��?��, ?��?��?���? 그에 맞는 콜백 ?��?���? ?��출해�??��
        glfwPollEvents();


        // UI �? �? ?��?��?��마다 그린?��
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame(); // �?금�???�� ?�� ?��?���? ?��?��?��?��?��

       
        // ?��?��?��?�� ?��?��?��?�� ?��?��?�� 처리
        context->ProcessInput(window);


        // Context Object ?��?�� => ?��면에 그리?�� ?��?��?�� 진행?��?��
        context->Render();


        // ?��?�� ?��?���? ?�� ?��, UI �? ?��?��링한?��
        ImGui::Render();    // 그려?�� ?��?�� ?��보�?? �??��?���?
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData()); // 그린?��


        /* 
            Double Buffering, Frame Buffer Swap

            Front buffer ??? Back Buffer �? ?���? 구현, 먼�?? back buffer ?�� 그린 ?��?��?��
            Front Buffer ??? Back Buffer �? 바꾼?��
         */
        glfwSwapBuffers(window);    // ?��?��?�� 버퍼 ?��?�� 코드
    }



    // ?��?��?���? 종료�? ?���?, ?��?��까�?? ?��?��?��?�� GL 리소?���? 모두 ?��리해?�� ?��?��
    // main ?��?�� 종료 ?��?��, 먼�?? GL 리소?�� ?��리�?? 명시?��?��
    //context.reset();  => Context obj �? �?리키?�� ?��마트 ?��?��?�� ?��?�� => ?��?��?�� 리소?�� ?���?
    context = nullptr; // nullptr ????��?���? ?��마트 ?��?��?���? ?��?��?�� ?��리한?��



    // 종료 ?��, ImGui ?�� ?��?��?�� 리소?���? 반환
    ImGui_ImplOpenGL3_DestroyFontsTexture();
    ImGui_ImplOpenGL3_DestroyDeviceObjects();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext(imguiContext);



    // glfw 리소?�� ?���?
    glfwTerminate();

    return 0;
}

/* 
    OpenGL Function 종류
    
    1. State Setting function
        OpenGL Context ?�� 값을 ????��?��?�� ?��?��
        glViewport(+)
        glClearColor(+)
        
    2. State Using function
        Context ?�� ????��?�� State �? ?��?��?��?�� ?��?��
        ?��?���? 그리거나 ?��????��?��
        glClear(+)
*/