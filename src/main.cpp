#include "context.h"


#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <spdlog/spdlog.h>

#include <glad/glad.h>
#include <glfw/glfw3.h>



// â ũ�Ⱑ ������ ��
void OnFramebufferSizeChange(GLFWwindow* window, int width, int height)
{
    SPDLOG_INFO("framebuffer size changed: ({} x {})", width, height);

    auto context = reinterpret_cast<Context*>(glfwGetWindowUserPointer(window));
    context->Reshape(width, height);
}

// Ű �Է� �߻� ��
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

    // SPACE Ű�� ó�� �ѹ� ������ �� => ���� �ߵ�
    if(key == GLFW_KEY_SPACE && action == GLFW_PRESS)
    {
        SPDLOG_INFO("Space Key Pressed");
        auto context = reinterpret_cast<Context*>(glfwGetWindowUserPointer(window));
        context->MakeJump();
    }

    // Left Shift �� ������ �� => �뽬 �ߵ�
    if(key == GLFW_KEY_LEFT_SHIFT && action == GLFW_PRESS)
    {
        SPDLOG_INFO("Left Shift Key Pressed");
        auto context = reinterpret_cast<Context*>(glfwGetWindowUserPointer(window));
        context->MakeDash();
    }


    // ESC => ���α׷� ����
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
}

// ���콺 ��ġ ���� ��
void OnCursorPos(GLFWwindow* window, double x, double y)
{
    auto context = (Context*)glfwGetWindowUserPointer(window);
    context->MouseMove(x, y);
}

// ���콺 ��ư
void OnMouseButton(GLFWwindow* window, int button, int action, int modifier)
{
    ImGui_ImplGlfw_MouseButtonCallback(window, button, action, modifier);

    auto context = (Context*)glfwGetWindowUserPointer(window);
    double x, y;
    // ���콺 Ŀ�� ���� ��ġ
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


    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);  //3.3 version �̿��µ���
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
    // ���⼭���� gl �Լ��� ����� �� �ִ�

    auto glVersion = glGetString(GL_VERSION);
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

    
    SPDLOG_INFO("Start main loop");



    double prevTime {}, curTime {};
    prevTime = glfwGetTime();

    while (!glfwWindowShouldClose(window))
    {
        curTime = glfwGetTime();       
        // 60FPS Rendering 
        if(curTime - prevTime >= 0.00166)
        {
            // Ű �Է��� ���� ���� ������
            glfwPollEvents();

            // IMGUI ����
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            // xz Ű �Է�, Ȯ��, �̵� ���� Ȯ��
            context->ProcessInput(window);

            // ���Ŀ� ������ ����
            context->Render();

            // IMGUI �׸���
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            // Double Buffering, Frame Buffer Swap
            glfwSwapBuffers(window);

            // �������� �ϰ� ����, prevTime ����
            prevTime = curTime;
        }
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