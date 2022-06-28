#include "context.h"


#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <spdlog/spdlog.h>

#include <glad/glad.h>
#include <glfw/glfw3.h>



// 창 크기가 변했을 때
void OnFramebufferSizeChange(GLFWwindow* window, int width, int height)
{
    SPDLOG_INFO("framebuffer size changed: ({} x {})", width, height);

    auto context = reinterpret_cast<Context*>(glfwGetWindowUserPointer(window));
    context->Reshape(width, height);
}

// 키 입력 발생 시
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

    // SPACE 키가 처음 한번 눌렸을 때
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

    // ESC => 프로그램 종료
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
}

// 마우스 위치 변경 시
void OnCursorPos(GLFWwindow* window, double x, double y)
{
    auto context = (Context*)glfwGetWindowUserPointer(window);
    context->MouseMove(x, y);
}

// 마우스 버튼
void OnMouseButton(GLFWwindow* window, int button, int action, int modifier)
{
    ImGui_ImplGlfw_MouseButtonCallback(window, button, action, modifier);

    auto context = (Context*)glfwGetWindowUserPointer(window);
    double x, y;
    // 마우스 커서 현재 위치
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


    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);  //3.3 version 이였는데요
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
    // 여기서부터 gl 함수를 사용할 수 있다

    auto glVersion = glGetString(GL_VERSION);
    SPDLOG_INFO("OpenGL context version: {}", glVersion);


    // imgui initialization
    // OpenGL context 瑜? 珥덇린?솕 ?븳 ?썑, ImGUI ?룄 珥덇린?솕瑜? ?븳?떎
    auto imguiContext = ImGui::CreateContext();     // imgui context
    ImGui::SetCurrentContext(imguiContext);         // current 濡? ?꽕?젙

    ImGui_ImplGlfw_InitForOpenGL(window, false);    // callback ?뿉 ????븳 ?꽕?젙??? false => 吏곸젒?븯寃좊떎
    ImGui_ImplOpenGL3_Init();
    ImGui_ImplOpenGL3_CreateFontsTexture();
    ImGui_ImplOpenGL3_CreateDeviceObjects();






    // ?궡媛? 留뚮뱶?뒗, 洹몃━湲? ?쐞?븳 Context
    // ?솕硫댁쓣 洹몃━?뒗 ?옉?뾽?쓣 吏꾪뻾?븯?뒗 Context obj ?깮?꽦
    auto context = Context::Create();
    if (!context) {
        SPDLOG_ERROR("failed to create context");
        glfwTerminate();
        return -1;
    }

    // ?쑀??? ?룷?씤?꽣 ?궗?슜
    // ?듅?젙 ?쐢?룄?슦?뿉 遺숈뿀?뒗 ?룷?씤?꽣瑜? 媛??졇?떎 ?벝 ?닔 ?엳?떎
    glfwSetWindowUserPointer(window, context.get());



    // 酉? ?룷?듃 ?깮?꽦?쓽 寃쎌슦, 泥섏쓬 ?쐢?룄?슦 ?깮?꽦 ?븣 ?궗?씠利덇?? 蹂??븯?뒗 => ?씠踰ㅽ듃媛? 諛쒖깮?븯吏? ?븡?쑝誘?濡?
    // 泥? ?샇異쒖?? 吏곸젒 ?븳?떎
    OnFramebufferSizeChange(window, WINDOW_WIDTH, WINDOW_HEIGHT);


    /* GLFW CallBack ?쓣 ?쐢?룄?슦?뿉 ?벑濡? */

    // ?봽?젅?엫 踰꾪띁瑜? ?옱?꽕?젙?븯?뒗 GLFW ?쓽 肄쒕갚?쓣 ?꽕?젙 => ?븵?쑝濡? GL ?씠 ?븣?븘?꽌 ?샇異쒗븯寃? ?맂?떎
    glfwSetFramebufferSizeCallback(window, OnFramebufferSizeChange);

    // ?궎 ?엯?젰?쓣 泥섎━?븯?뒗 GLFW 肄쒕갚 ?꽕?젙
    glfwSetKeyCallback(window, OnKeyEvent);
    	
    glfwSetCharCallback(window, OnCharEvent);

    glfwSetCursorPosCallback(window, OnCursorPos);
    	
    glfwSetMouseButtonCallback(window, OnMouseButton);

    glfwSetScrollCallback(window, OnScroll);

    // glfw 硫붿씤 猷⑦봽 ?떎?뻾, ?쐢?룄?슦 close 踰꾪듉?쓣 ?늻瑜대㈃ ?젙?긽 醫낅즺
    SPDLOG_INFO("Start main loop");
    while (!glfwWindowShouldClose(window)) {
        
        // window ?뿉?꽌 諛쒖깮?븯?뒗 ?씠踰ㅽ듃瑜? ?닔吏?
        // ?씠踰ㅽ듃 諛쒖깮 ?떆?뿉, ?옄?룞?쑝濡? 洹몄뿉 留욌뒗 肄쒕갚 ?븿?닔瑜? ?샇異쒗빐以??떎
        glfwPollEvents();


        // UI 瑜? 留? ?봽?젅?엫留덈떎 洹몃┛?떎
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame(); // 吏?湲덈???꽣 ?깉 ?젋?뜑留? ?봽?젅?엫?씠?떎

       
        // ?쐢?룄?슦?뿉 ?뱾?뼱?삤?뒗 ?씤?뭼?쓣 泥섎━
        context->ProcessInput(window);


        // Context Object ?씠?슜 => ?솕硫댁뿉 洹몃━?뒗 ?옉?뾽?쓣 吏꾪뻾?븳?떎
        context->Render();


        // ?뵮?쓣 ?젋?뜑留? ?븳 ?썑, UI 瑜? ?젋?뜑留곹븳?떎
        ImGui::Render();    // 洹몃젮?빞 ?븯?뒗 ?젙蹂대?? 媛??졇?삤怨?
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData()); // 洹몃┛?떎


        /* 
            Double Buffering, Frame Buffer Swap

            Front buffer ??? Back Buffer 瑜? ?뵲猷? 援ы쁽, 癒쇱?? back buffer ?뿉 洹몃┛ ?떎?쓬?뿉
            Front Buffer ??? Back Buffer 瑜? 諛붽씔?떎
         */
        glfwSwapBuffers(window);    // ?봽?젅?엫 踰꾪띁 ?뒪?솑 肄붾뱶
    }



    // ?쐢?룄?슦媛? 醫낅즺媛? ?릺硫?, ?씠?젣源뚯?? ?궗?슜?뻽?뜕 GL 由ъ냼?뒪瑜? 紐⑤몢 ?젙由ы빐?빞 ?븳?떎
    // main ?븿?닔 醫낅즺 ?쟾?뿉, 癒쇱?? GL 由ъ냼?뒪 ?젙由щ?? 紐낆떆?븳?떎
    //context.reset();  => Context obj 瑜? 媛?由ы궎?뜕 ?뒪留덊듃 ?룷?씤?꽣 ?씠?슜 => ?븷?떦?븳 由ъ냼?뒪 ?젙由?
    context = nullptr; // nullptr ????엯?븯硫? ?뒪留덊듃 ?룷?씤?꽣媛? ?븣?븘?꽌 ?젙由ы븳?떎



    // 醫낅즺 ?쟾, ImGui ?뿉 ?븷?떦?븳 由ъ냼?뒪瑜? 諛섑솚
    ImGui_ImplOpenGL3_DestroyFontsTexture();
    ImGui_ImplOpenGL3_DestroyDeviceObjects();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext(imguiContext);



    // glfw 由ъ냼?뒪 ?젙由?
    glfwTerminate();

    return 0;
}

/* 
    OpenGL Function 醫낅쪟
    
    1. State Setting function
        OpenGL Context ?뿉 媛믪쓣 ????옣?븯?뒗 ?븿?닔
        glViewport(+)
        glClearColor(+)
        
    2. State Using function
        Context ?뿉 ????옣?맂 State 瑜? ?씠?슜?븯?뒗 ?븿?닔
        ?떎?젣濡? 洹몃━嫄곕굹 ?굹????궦?떎
        glClear(+)
*/