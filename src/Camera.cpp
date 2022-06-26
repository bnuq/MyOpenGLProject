#include "Camera.h"

CameraPtr Camera::Create(PlayerPtr player)
{
    auto camera = CameraPtr(new Camera());
    camera->MainPlayer = player;
    return std::move(camera);
}




void Camera::GetPosition()
{
    Position = MainPlayer->Position + (-MainPlayer->FrontVec) * xzDist + MainPlayer->UpVec * yDist;
}


