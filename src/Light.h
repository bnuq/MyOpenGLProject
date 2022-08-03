#ifndef __LIGHT_H__
#define __LIGHT_H__

#include "../FrameWork/common.h"


CLASS_PTR(Light)
class Light
{
public:

    // 광원의 위치
    glm::vec3 Position;

    // 광원이 캐릭터로부터 떨어지는 상대적 위치를 나타내는 벡터
    float FarWidth    = 5.0f;

    // 광원이 바라보는 방향
    glm::vec3 Direction = glm::normalize(glm::vec3(-FarWidth, -FarWidth, -FarWidth));


    bool LightMove              = false;
    float LightMoveSpeed        = 0.08f;

    glm::vec3 ambient { glm::vec3(0.8f, 0.8f, 0.8f) };
    glm::vec3 diffuse { glm::vec3(0.8f, 0.8f, 0.8f) };
    glm::vec3 specular { glm::vec3(1.0f, 1.0f, 1.0f) };
    


    // Light 클립 공간을 만들 때 orthogonal 영역을 지정
    // 해당 영역에 대한 값
    float minusX = -FarWidth;
    float plusX = FarWidth;
    
    float minusY = -FarWidth;
    float plusY = FarWidth;

    float nearZ = 0.0f;
    float farZ = FarWidth * 10;



    // Constructor
    /* 
        캐릭터는 원점 (0, 0, 0) 에서 생성

        광원은 항상 캐릭터의 좌측 상단에 위치한다고 가정
            캐릭터가 어디있던지, 캐릭터 기준으로 항상 동일한 위치에 있다
            캐릭터가 움직일 때, 근거리에서 계속 캐릭터를 따라다닌다고 가정

        어차피 directional light 라 가정하기 때문에, 광원의 실제 위치는 중요하지 않다
        다만 그림자를 만들기 위한 orthogonal matrix 정의를 위해서 광원은 캐릭터를 계속 따라다녀야 한다
        
        광원의 orthogonal matrix 안에는 항상 캐릭터가 담기게 된다
     */
    Light(glm::vec3 charPos)
    : Position(charPos + glm::vec3(FarWidth, FarWidth, FarWidth))
    {

    }


    // 캐릭터와 떨어진 거리를 파악, 광원의 위치를 재조정한다
    void SetPosition(glm::vec3 charPos);

};

#endif  // __LIGHT_H__