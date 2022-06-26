#include "Camera.h"

CameraPtr Camera::Create(PlayerPtr player)
{
    auto camera = CameraPtr(new Camera());
    camera->MainPlayer = player;
    return std::move(camera);
}



void Camera::SetPosition(float HEIGHT)
{
    auto MainPos = MainPlayer->Position;
    MainPos.y = HEIGHT;
    Position = MainPos + (-MainPlayer->FrontVec) * xzDist + MainPlayer->UpVec * yDist;
}



void Camera::SetDirection()
{
    Direction = glm::vec3(glm::rotate(glm::mat4(1.0f), glm::radians(pitch), MainPlayer->LeftVec)
                        * glm::vec4(MainPlayer->FrontVec, 0.0f));
}



void Camera::Rotate(float angle)
{
    pitch = glm::clamp(pitch + angle * rotSpeed, rotLimit.x, rotLimit.y);
}