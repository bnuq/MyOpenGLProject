#ifndef __CAM_H__
#define __CAM_H__

#include "../FrameWork/common.h"
#include "Character.h"


// 카메라의 정보를 가지고 있는 카메라 객체
CLASS_PTR(Camera)
class Camera
{
public:
    // 피벗 = 카메라가 바라보는 점이자 회전의 중심
    glm::vec3 CameraPivot;
        float CamToCharMaxLimit = 0.1f;                         // 카메라 피벗이 움직이게 하는 거리
        float CamToCharMinLimit = 0.01f;                        // 카메라 피벗이 더이상 움직이지 않게 하는 거리
        bool CameraMove = false;                                // 카메라가 움직이고 있는 지 ~ 상태

    
    // 카메라 이동
    float CameraMoveSpeed = 0.05f;

    
    // 카메라 회전
    float yawRotSpeed { 0.1f };
    float pitchRotSpeed { 0.1f };
        glm::vec2 pitchRotLimit { glm::vec2{-10.0f, 30.0f} };



    // 카메라의 위치
    glm::vec3 Position;

    // 카메라 모델 좌표계
    glm::vec3 FrontVec          = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::vec3 LeftVec           = glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 UpVec             = glm::vec3(0.0f, 1.0f, 0.0f);

    // 카메라가 바라보는 방향
    glm::vec3 Direction         = glm::vec3(0.0f, 0.0f, 1.0f);
    


    // 카메라가 따라다니는 캐릭터
    CharacterPtr mainChar;
        // 카메라가 캐릭터로 부터 떨어진 거리 
        float xzDist = 20.0f;
        float yDist = 20.0f;


    // 생성 및 초기화
    static CameraPtr Create(CharacterPtr player);
    void Init(CharacterPtr player);
    

    // 움직인 캐릭터에 맞춰, 카메라를 이동시킴
    void SetPosition();


    // 마우스를 움직이는 만큼, 카메라가 회전한다 + 바라보는 Direction 벡터도 달라진다
    void Rotate(glm::vec2 deltaPos);


private:
    Camera() {}
};

#endif  // __CAM_H__