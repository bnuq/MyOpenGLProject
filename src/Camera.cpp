#include "Camera.h"

CameraPtr Camera::Create(CharacterPtr player)
{
    auto camera = CameraPtr(new Camera());

    camera->mainChar = player;
    camera->SetPosition();

    return camera;
}

/* 
    glm::rotate 
        => 임의의 축을 중심으로 회전하는 쿼터니언을 생성하는 건 맞는데
        쿼터니언 = 회전을 복소수를 통해서 표현한 것
        원점을 지나는 축에 대한 회전을 나타낸다

        따라서 이번 경우처럼 특정 pivot 을 중심으로 회전을 하는 경우
        원점에서의 회전으로 변경한 다음, 회전하고 다시 원상복구하는 과정이 필요하다
 */
// 카메라의 위치를 계산하는 함수 => 각도까지 모두 계산한다
void Camera::SetPosition()
{
    auto MainPos = mainChar->Position;
    if(!mainChar->Falling)
        MainPos.y = mainChar->groundHeight;

    // 위치 세팅
    Position = MainPos + (glm::vec3(0.0f, 0.0f, -1.0f)) * xzDist + (glm::vec3(0.0f, 1.0f, 0.0f)) * yDist;
    
    auto goToOrigin = glm::translate(glm::mat4(1.0f), -(MainPos));
    auto yawRotate = glm::rotate(glm::mat4(1.0f), glm::radians(yawAngle), glm::vec3(0.0f, 1.0f, 0.0f));
    auto comeBack = glm::translate(glm::mat4(1.0f), MainPos);
    
    Position = glm::vec3((comeBack * yawRotate * goToOrigin) * glm::vec4(Position, 1.0f));
    
    // 좌표계 세팅
    FrontVec = glm::normalize(mainChar->Position - Position);
    FrontVec.y = 0.0f;
    LeftVec = glm::cross(UpVec, FrontVec);
    Direction = FrontVec;

    // Direction 세팅
    auto pitchRotate = glm::rotate(glm::mat4(1.0f), glm::radians(pitchAngle), LeftVec); // 바뀐 LeftVec 에 대해서
    Direction = glm::vec3(pitchRotate * glm::vec4(Direction, 0.0f));
}



// 카메라를 회전 시키는 역할 => 각도만 업데이트 한다
void Camera::Rotate(glm::vec2 deltaPos)
{
    yawAngle += (-deltaPos.x) * yawRotSpeed;
    if(yawAngle < 0)    yawAngle += 360;
    if(yawAngle > 360)  yawAngle -= 360;

    // pitchAngle 은 제한이 있다 + 값을 누적한다
    pitchAngle += (deltaPos.y) * pitchRotSpeed;
    pitchAngle = glm::clamp(pitchAngle, pitchRotLimit.x, pitchRotLimit.y);
}