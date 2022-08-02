#ifndef __LIGHT_H__
#define __LIGHT_H__

#include "../FrameWork/common.h"


CLASS_PTR(Light)
class Light
{
public:

    // 광원의 위치
    glm::vec3 Position          = glm::vec3(10.0f, 10.0f, 10.0f);

    // 광원이 바라보는 방향
    glm::vec3 Direction         = glm::vec3(-1.0f, -1.0f, -1.0f);

    bool LightMove              = false;
    float yLimitLengthMax       = 15.0f;    // y 축 성분 간 떨어진 거리가 일정 이상이면 이동한다
    float yLimitLengthMin       = 10.0f;    // y 축 성분 간 거리가 이 이하가 될 때까지 이동한다
    float LightMoveSpeed        = 0.08f;

    glm::vec3 ambient { glm::vec3(0.4f, 0.4f, 0.4f) };
    glm::vec3 diffuse { glm::vec3(0.8f, 0.8f, 0.8f) };
    glm::vec3 specular { glm::vec3(1.0f, 1.0f, 1.0f) };
    


    // Light 클립 공간을 만들 때 orthogonal 영역을 지정
    // 해당 영역에 대한 값
    float minusX    = -50.0f;
    float plusX     = +50.0f;
    
    float minusY    = -50.0f;
    float plusY     = +50.0f;

    float nearZ     = -5.0f;
    float farZ      = +5.0f;



    // 캐릭터와 떨어진 거리를 파악, 광원의 위치를 재조정한다
    void SetPosition(float charPosY);

};

#endif  // __LIGHT_H__