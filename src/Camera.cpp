#include "Camera.h"


CameraPtr Camera::Create(CharacterPtr player)
{
    auto camera = CameraPtr(new Camera());
    camera->Init(player);
    return camera;
}



// 카메라 초기화
void Camera::Init(CharacterPtr player)
{
    mainChar = player;

    // 피벗 위치 = 메인 캐릭터 위치
    CameraPivot = mainChar->Position;

    // 좌표계 초기화
    // 카메라 모델 좌표계
    FrontVec = glm::normalize(mainChar->FrontVec);
    LeftVec  = glm::normalize(mainChar->LeftVec);
    UpVec    = glm::normalize(mainChar->UpVec);

    // 카메라 위치 초기화
    Position = CameraPivot + (-FrontVec) * xzDist + UpVec * yDist;

    // 카메라가 바라보는 방향 = 캐릭터를 바라보는 방향
    Direction = glm::normalize(CameraPivot - Position);
}



// 카메라의 위치를 계산하는 함수 => 피벗의 위치를 메인 캐릭터에 맞춰서 이동
void Camera::SetPosition()
{
    // 메인 캐릭터와 피벗 사이의 벡터
    auto PivotToChar = mainChar->Position - CameraPivot;
    
    // 카메라가 움직이지 않는 상태였고 &&
    // 캐릭터 ~ 피벗 사이의 길이가 일정 이하면 움직이지 않는다
    if(!CameraMove && glm::length(PivotToChar) < CamToCharMaxLimit)
        return;
    


    // 움직이기로 결정했으면 ~ 캐릭터와 피벗 사이의 거리가 일정 이하가 될 때까지 카메라가 움직니다
    CameraMove = true;

    // 카메라 속도로 메인 카메라를 따라간다
    CameraPivot += PivotToChar * CameraMoveSpeed;
    Position += PivotToChar * CameraMoveSpeed;

    // 일정 값 이하로 가까워지면, 다시 카메라를 고정시킨다
    PivotToChar = mainChar->Position - CameraPivot;
    if(glm::length(PivotToChar) < CamToCharMinLimit)
        CameraMove = false;
}



/* 
    glm::rotate 
        => 임의의 축을 중심으로 회전하는 쿼터니언을 생성하는 건 맞는데
        쿼터니언 = 회전을 복소수를 통해서 표현한 것
        원점을 지나는 축에 대한 회전을 나타낸다

        따라서 이번 경우처럼 특정 pivot 을 중심으로 회전을 하는 경우
            피벗 -> 원점 으로 이동
            원점에서 회전하고 
            다시 원점 -> 피벗 으로 원상복구
        하는 과정이 필요하다
 */
// 카메라를 회전 시키는 함수 => 피벗 기준으로 회전한다
void Camera::Rotate(glm::vec2 deltaPos)
{
    // Yaw => 카메라의 Up Vector 기준 회전 => 무조건 (0, 1, 0) 기준 회전
    auto yawAngle = (-deltaPos.x) * yawRotSpeed;
        if(yawAngle < 0)    yawAngle += 360;
        if(yawAngle > 360)  yawAngle -= 360;

    auto goToOrigin = glm::translate(glm::mat4(1.0f), -(CameraPivot));
    auto yawRotate = glm::rotate(glm::mat4(1.0f), glm::radians(yawAngle), mainChar->UpVec);
    auto comeBack = glm::translate(glm::mat4(1.0f), CameraPivot);

    Position = glm::vec3((comeBack * yawRotate * goToOrigin) * glm::vec4(Position, 1.0f));

    // 카메라 좌표계도 회전시켜 주어야 하는데, 그냥 회전 변환 적용하면 된다
    FrontVec = glm::vec3(yawRotate * glm::vec4(FrontVec, 0.0f));
    LeftVec = glm::vec3(yawRotate * glm::vec4(LeftVec, 0.0f));
    Direction = glm::vec3(yawRotate * glm::vec4(Direction, 0.0f));


    /* ****** */


    // Pitch => 카메라의 Left Vector 기준 회전 => 오른손 좌표계 사용
    // pitchAngle 은 제한이 있다 + 값을 누적한다 => 왜? 항상 정면을 바라본 상태에서 어느정도 각도로 회전하느냐를 따지니까
    auto pitchAngle = (deltaPos.y) * pitchRotSpeed;
        pitchAngle = glm::clamp(pitchAngle, pitchRotLimit.x, pitchRotLimit.y);

    // 좌표계를 회전하지 않고, Direction 벡터를 회전시킨다
    auto pitchRotate = glm::rotate(glm::mat4(1.0f), glm::radians(pitchAngle), LeftVec); // 바뀐 LeftVec 에 대해서
    Direction = glm::vec3(pitchRotate * glm::vec4(Direction, 0.0f));
}