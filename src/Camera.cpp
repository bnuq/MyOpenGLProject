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
        => ������ ���� �߽����� ȸ���ϴ� ���ʹϾ��� �����ϴ� �� �´µ�
        ���ʹϾ� = ȸ���� ���Ҽ��� ���ؼ� ǥ���� ��
        ������ ������ �࿡ ���� ȸ���� ��Ÿ����

        ���� �̹� ���ó�� Ư�� pivot �� �߽����� ȸ���� �ϴ� ���
        ���������� ȸ������ ������ ����, ȸ���ϰ� �ٽ� ���󺹱��ϴ� ������ �ʿ��ϴ�
 */
// ī�޶��� ��ġ�� ����ϴ� �Լ� => �������� ��� ����Ѵ�
void Camera::SetPosition()
{
    auto MainPos = mainChar->Position;
    if(!mainChar->Falling)
        MainPos.y = mainChar->groundHeight;

    // ��ġ ����
    Position = MainPos + (glm::vec3(0.0f, 0.0f, -1.0f)) * xzDist + (glm::vec3(0.0f, 1.0f, 0.0f)) * yDist;
    
    auto goToOrigin = glm::translate(glm::mat4(1.0f), -(MainPos));
    auto yawRotate = glm::rotate(glm::mat4(1.0f), glm::radians(yawAngle), glm::vec3(0.0f, 1.0f, 0.0f));
    auto comeBack = glm::translate(glm::mat4(1.0f), MainPos);
    
    Position = glm::vec3((comeBack * yawRotate * goToOrigin) * glm::vec4(Position, 1.0f));
    
    // ��ǥ�� ����
    FrontVec = glm::normalize(mainChar->Position - Position);
    FrontVec.y = 0.0f;
    LeftVec = glm::cross(UpVec, FrontVec);
    Direction = FrontVec;

    // Direction ����
    auto pitchRotate = glm::rotate(glm::mat4(1.0f), glm::radians(pitchAngle), LeftVec); // �ٲ� LeftVec �� ���ؼ�
    Direction = glm::vec3(pitchRotate * glm::vec4(Direction, 0.0f));
}



// ī�޶� ȸ�� ��Ű�� ���� => ������ ������Ʈ �Ѵ�
void Camera::Rotate(glm::vec2 deltaPos)
{
    yawAngle += (-deltaPos.x) * yawRotSpeed;
    if(yawAngle < 0)    yawAngle += 360;
    if(yawAngle > 360)  yawAngle -= 360;

    // pitchAngle �� ������ �ִ� + ���� �����Ѵ�
    pitchAngle += (deltaPos.y) * pitchRotSpeed;
    pitchAngle = glm::clamp(pitchAngle, pitchRotLimit.x, pitchRotLimit.y);
}