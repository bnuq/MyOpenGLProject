# 최소 cmake 3.13 버전 이상에서 작동
cmake_minimum_required(VERSION 3.13)



# 변수 세팅
set(PROJECT_NAME opengl_example)  # CMake Project 이름
set(CMAKE_CXX_STANDARD 17)        # C++ 버전 설정



# 윈도우 생성에 필요한 변수 설정
set(WINDOW_NAME "OpenGL Example") # 윈도우 창 이름
set(WINDOW_WIDTH 960)             # 윈도우 너비
set(WINDOW_HEIGHT 540)            # 윈도우 높이



# CMake Project 이름 설정
project(${PROJECT_NAME})



#[[
    EXE 실행 파일을 생성하는 함수
    여러 소스파일들을 컴파일 => 빌드 => 실행파일을 만든다
    실행파일을 만드는 데 필요한 모든 파일들을 지정한다
]]#
add_executable(
    ${PROJECT_NAME}                               # Project 이름과 동일한 실행파일을 생성
    src/main.cpp

    src/common.cpp              src/common.h

    src/shader.cpp              src/shader.h
    src/program.cpp             src/program.h

    src/buffer.cpp              src/buffer.h
    src/vertex_layout.cpp       src/vertex_layout.h

    src/image.cpp               src/image.h
    src/texture.cpp             src/texture.h

    src/context.cpp             src/context.h
    
    src/mesh.cpp                src/mesh.h
    src/model.cpp               src/model.h

    src/framebuffer.cpp         src/framebuffer.h

    src/Player.cpp              src/Player.h
)



# 외부 라이브버리를 가져오는 Custom Module 을 실행
include(Dependency.cmake)



#[[
    프로젝트 내 소스파일에서 외부 라이브러리의 함수를 사용할 수 있어야 한다
    
    외부 라이브러리의 헤더 파일에 접근 => 실행 파일을 컴파일
    => 외부 라이브러리와 실행 파일을 링크

    ${DEP_INCLUDE_DIR} = ./install/include
    해당 디렉토리를 실행 파일 = Target 에 포함시켜서
    => 소스 파일에서 해더 파일에 접근할 수 있도록 한다
]]#
target_include_directories(${PROJECT_NAME} PUBLIC ${DEP_INCLUDE_DIR})



#[[
    ${DEP_LIB_DIR} = ./install/lib = 외부 라이브버리가 저장된 디렉토리
    실행 파일의 링크 단계에서, 해당 디렉토리를 참고하도록 알림
]]#
target_link_directories(${PROJECT_NAME} PUBLIC ${DEP_LIB_DIR})



# 실행 파일에 실제 외부 라이브러리를 link => Linking 과정 실행
target_link_libraries(${PROJECT_NAME} PUBLIC ${DEP_LIBS})



#[[
    main.cpp 에서 컴파일을 할 때 사용할 수 있는 매크로
    #define 을 소스파일 바깥에서 정의하는 역할

    소스파일에서 CMakeLists 에서 정의한 변수를 사용할 수 있게 한다
]]#
target_compile_definitions(
    ${PROJECT_NAME}
    PUBLIC
    WINDOW_NAME="${WINDOW_NAME}"
    WINDOW_WIDTH=${WINDOW_WIDTH}
    WINDOW_HEIGHT=${WINDOW_HEIGHT}
)



#[[
    Target 간의 Dependency 를 설정하는 함수
        누가 누구에게 의존하는 지를 설정
        ${DEP_LIST} = 외부 라이브러리 패키지 이름들의 list
    
    실행 파일이 외부 라이브러리에 의존하고 있으므로
    프로젝트를 컴파일 하기 전에, Dependency들이 먼저 build 될 수 있게 설정

    라이브러리를 먼저 build 해야 .lib 파일이 생성된다 => 내 프로젝트가 이후 링크할 수 있다
    하나의 라이브러리를 구성하는 여러 obj files => 하나로 묶으면 => lib file

    이후 프로젝트를 컴파일 한 후, 라이브러리와 링크~
    외부 라이브러리 리스트를 통해 빌드 해야 하는 외부 라이브러리들을 파악할 수 있다
]]#
add_dependencies(${PROJECT_NAME} ${DEP_LIST})