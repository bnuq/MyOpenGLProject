#ifndef __CHAR_H__
#define __CHAR_H__

#include "../FrameWork/common.h"

// 플레이어가 조작하는 캐릭터
CLASS_PTR(Character)
class Character
{
public:
    // 위치
    glm::vec3 Position;

    // 캐릭터가 닿을 것으로 추정하는 바닥
    unsigned int CharStory = 0;
    
    // 캐릭터는 Box 형태를 가진다 => 각 방향별 scale 값 존재
    float xScale;
    float yScale;
    float zScale;

    // 캐릭터의 모델 좌표계
    glm::vec3 FrontVec          = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::vec3 LeftVec           = glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 UpVec             = glm::vec3(0.0f, 1.0f, 0.0f);



    // Constructor 
    Character(glm::vec3 pos, float x = 1.0f, float y = 1.0f, float z = 1.0f)
    : Position(pos), xScale(x), yScale(y), zScale(z)
    {}


    // 키 입력 => XZ 평면에서의 이동 여부와 이동 방향을 구함
    void GetXZDir(glm::vec3 dir);
        glm::vec3 xzDir{};
        bool xzMoving = false;


    // 키 입력에 따른 움직임
    void Move();
        /* XZ 평면 이동 관련 */
        float MoveSpeed = 1.5f;
        const float MoveSpeedRatio = 0.1f;

        /* Y 축 이동 관련 */
        float ySpeed = 0.0f;                    // 누적 y 축 방향 이동속도
        float PrevHeight;

        float Gravity = 1.0f;                   // 중력은 항상 아래로 작용, 양수 값을 가지게 하고 그걸 빼는 방식으로 작동
        const float GravityRatio = 0.001f;


    // y 축 상의 움직임
    void yStop();
    void OnAir();


    // 캐릭터가 움직이는 방향에 맞게, 캐릭터를 회전
    void Rotate();
        float YawAngleTick  = 1.0f;
        const float YawAngleTickRatio = 1.0f;
        float YawMinValue = 0.01f;


    // 점프 처리
    void Jump();
        bool OnGround = false;                  // 떨어지고 있는 지 체크 => 이중 점프를 막는데 사용
        float JumpPower = 1.0f;
        const float JumpPowerRatio = 0.1f;
};


#endif // __CHAR_H__