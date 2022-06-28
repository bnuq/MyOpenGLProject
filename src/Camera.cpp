#include "Camera.h"

CameraPtr Camera::Create(CharacterPtr player)
{
    auto camera = CameraPtr(new Camera());
    camera->mainChar = player;
    return camera;
}



void Camera::SetPosition(float HEIGHT)
{
    auto MainPos = mainChar->Position;
    MainPos.y = HEIGHT;
    Position = MainPos + (-mainChar->FrontVec) * xzDist + mainChar->UpVec * yDist;
}



void Camera::SetDirection()
{
    Direction = glm::vec3(glm::rotate(glm::mat4(1.0f), glm::radians(pitch), mainChar->LeftVec)
                        * glm::vec4(mainChar->FrontVec, 0.0f));
}



void Camera::Rotate(float angle)
{
    pitch = glm::clamp(pitch + angle * rotSpeed, rotLimit.x, rotLimit.y);
}