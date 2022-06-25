#pragma warning(disable: 4819)



/* 
    화면에 그려내는 작업 관련된 것들을
        GL 함수, GL 객체
    main 함수에 코드를 드러내지 않고
    Context Class 를 이용해서 관리한다
*/
#include "context.h"

// imgui 헤더파일 추가
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>



// 외부 라이브러리
#include <spdlog/spdlog.h>
// glad 라이브러리, GLFW 이전에 추가
#include <glad/glad.h>
#include <GLFW/glfw3.h>



/* GLFW 가 CallBack 으로 사용할 함수들을 정의 */
// 윈도우 사이즈 변경 시, 뷰 포트 크기를 재정의하는 함수
void OnFramebufferSizeChange(GLFWwindow* window, int width, int height) {
    SPDLOG_INFO("framebuffer size changed: ({} x {})", width, height);
    /* 
        glViewport

        OpenGL 이 그림을 그릴 화면의 위치 및 크기 설정
        만들어진 윈도우 내에서 설정
     */
    
    //glViewport(0, 0, width, height);

    // 유저 포인터를 통해서, 윈도우에 속한 컨택스트 에 접근한다
    auto context = reinterpret_cast<Context*>(glfwGetWindowUserPointer(window));
    // 뷰 포트 사이즈를 재조정하면서, 프로젝션의 종횡비도 재조정
    context->Reshape(width, height);
}
// 키보드 입력을 처리하는 함수
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
        context->PlayerJump();
    }
    if (key == GLFW_KEY_SPACE && action == GLFW_RELEASE)
    {
        //SPDLOG_INFO("Space Key Released");
        //auto context = reinterpret_cast<Context*>(glfwGetWindowUserPointer(window));
        //context->PlayerGround();
    }


    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

void OnCursorPos(GLFWwindow* window, double x, double y) {
    // 유저 포인터 사용 => 윈도우의 context 에 접근
    auto context = (Context*)glfwGetWindowUserPointer(window);
    context->MouseMove(x, y);
}

void OnMouseButton(GLFWwindow* window, int button, int action, int modifier) {

    ImGui_ImplGlfw_MouseButtonCallback(window, button, action, modifier);

    auto context = (Context*)glfwGetWindowUserPointer(window);
    double x, y;

    // 커서의 위치 가져옴
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
        새롭게 speed log 를 이용
        시간 출력
        라인 번호 출력
    */    
    SPDLOG_INFO("Start Program");


    SPDLOG_INFO("Initialize glfw");
    /*
        glfwInit() => glfw 라이브러리 자체를 초기화

        미리 glfw 라이브러리에 정의되어 있는 변수들을 만들고, 초기화 한다
        glfw 라이브러리가 사용하는 리소스가 존재하기 때문에, 이들을 할당하고 생성하는 단계
            리소스니까, glfw functions, glfw variables 를 사용하기 위해서
            반드시 필요

        리소스를 할당하기 때문에, 반드시 종료 전 리소스를 해제하는 작업도 필요하다
            glfwTerminate()
    */
    if (!glfwInit()) {    // glfw 라이브러리 초기화, 실패하면 에러 출력후 종료
        const char* description = nullptr;
        glfwGetError(&description);
        SPDLOG_ERROR("failed to initialize glfw: {}", description);
        return -1;
    }




    /* 
        OpenGL 초기화
            1. GLFW 윈도우 샏성 전 희망하는 opengl 버전 추가
            사용하려는 버전 힌트
     */
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);  //3.3 version
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    // glfw 윈도우 생성
    SPDLOG_INFO("Create glfw window");
    auto window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_NAME, nullptr, nullptr);
    if (!window) {
        SPDLOG_ERROR("failed to create glfw window");
        glfwTerminate();
        return -1;
    }


    /* 
        OpenGL 초기화
            2. 윈도우 생성 후, 해당 윈도우에는 OpenGL Context 라는 게 생성된다
            만들어진 윈도우의 Context 를 사용하겠다, 해당 윈도우에서 그리겠다 라는 것을 알려야 한다
     */
    glfwMakeContextCurrent(window);


    /* 
        OpenGL 초기화
            3. glad 라이브러리를 통해서 -> 각 그래픽 카드에 맞는 OpenGL 함수를 로딩
            OpenGL Context 생성 이후 실행
            윈도우를 통해 만들어진 OpenGL Context 를 사용한다
     */
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        SPDLOG_ERROR("failed to initialize glad");
        glfwTerminate();
        return -1;
    }
    //------ 이제 이 밑에서부터, OpenGL 함수를 사용할 수 있다 ------//



    auto glVersion = glGetString(GL_VERSION); //생성된 OpenGL Context 실제 버전
    SPDLOG_INFO("OpenGL context version: {}", glVersion);


    // imgui initialization
    // OpenGL context 를 초기화 한 후, ImGUI 도 초기화를 한다
    auto imguiContext = ImGui::CreateContext();     // imgui context
    ImGui::SetCurrentContext(imguiContext);         // current 로 설정

    ImGui_ImplGlfw_InitForOpenGL(window, false);    // callback 에 대한 설정은 false => 직접하겠다
    ImGui_ImplOpenGL3_Init();
    ImGui_ImplOpenGL3_CreateFontsTexture();
    ImGui_ImplOpenGL3_CreateDeviceObjects();






    // 내가 만드는, 그리기 위한 Context
    // 화면을 그리는 작업을 진행하는 Context obj 생성
    auto context = Context::Create();
    if (!context) {
        SPDLOG_ERROR("failed to create context");
        glfwTerminate();
        return -1;
    }

    // 유저 포인터 사용
    // 특정 윈도우에 붙었는 포인터를 가져다 쓸 수 있다
    glfwSetWindowUserPointer(window, context.get());



    // 뷰 포트 생성의 경우, 처음 윈도우 생성 때 사이즈가 변하는 => 이벤트가 발생하지 않으므로
    // 첫 호출은 직접 한다
    OnFramebufferSizeChange(window, WINDOW_WIDTH, WINDOW_HEIGHT);


    /* GLFW CallBack 을 윈도우에 등록 */

    // 프레임 버퍼를 재설정하는 GLFW 의 콜백을 설정 => 앞으로 GL 이 알아서 호출하게 된다
    glfwSetFramebufferSizeCallback(window, OnFramebufferSizeChange);

    // 키 입력을 처리하는 GLFW 콜백 설정
    glfwSetKeyCallback(window, OnKeyEvent);
    	
    glfwSetCharCallback(window, OnCharEvent);

    glfwSetCursorPosCallback(window, OnCursorPos);
    	
    glfwSetMouseButtonCallback(window, OnMouseButton);

    glfwSetScrollCallback(window, OnScroll);

    // glfw 메인 루프 실행, 윈도우 close 버튼을 누르면 정상 종료
    SPDLOG_INFO("Start main loop");
    while (!glfwWindowShouldClose(window)) {
        
        // window 에서 발생하는 이벤트를 수집
        // 이벤트 발생 시에, 자동으로 그에 맞는 콜백 함수를 호출해준다
        glfwPollEvents();


        // UI 를 매 프레임마다 그린다
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame(); // 지금부터 새 렌더링 프레임이다

       
        // 윈도우에 들어오는 인풋을 처리
        context->ProcessInput(window);


        // Context Object 이용 => 화면에 그리는 작업을 진행한다
        context->Render();


        // 씬을 렌더링 한 후, UI 를 렌더링한다
        ImGui::Render();    // 그려야 하는 정보를 가져오고
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData()); // 그린다


        /* 
            Double Buffering, Frame Buffer Swap

            Front buffer 와 Back Buffer 를 따루 구현, 먼저 back buffer 에 그린 다음에
            Front Buffer 와 Back Buffer 를 바꾼다
         */
        glfwSwapBuffers(window);    // 프레임 버퍼 스왑 코드
    }



    // 윈도우가 종료가 되면, 이제까지 사용했던 GL 리소스를 모두 정리해야 한다
    // main 함수 종료 전에, 먼저 GL 리소스 정리를 명시한다
    //context.reset();  => Context obj 를 가리키던 스마트 포인터 이용 => 할당한 리소스 정리
    context = nullptr; // nullptr 대입하면 스마트 포인터가 알아서 정리한다



    // 종료 전, ImGui 에 할당한 리소스를 반환
    ImGui_ImplOpenGL3_DestroyFontsTexture();
    ImGui_ImplOpenGL3_DestroyDeviceObjects();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext(imguiContext);



    // glfw 리소스 정리
    glfwTerminate();

    return 0;
}

/* 
    OpenGL Function 종류
    
    1. State Setting function
        OpenGL Context 에 값을 저장하는 함수
        glViewport(+)
        glClearColor(+)
        
    2. State Using function
        Context 에 저장된 State 를 이용하는 함수
        실제로 그리거나 나타낸다
        glClear(+)
*/