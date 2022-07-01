#include "Camera.h"

CameraPtr Camera::Create(CharacterPtr player)
{
    auto camera = CameraPtr(new Camera());
    camera->Init(player);
    return camera;
}


// ī�޶� �ʱ�ȭ
void Camera::Init(CharacterPtr player)
{
    mainChar = player;

    // �ǹ� ��ġ = ���� ĳ���� ��ġ
    CameraPivot = mainChar->Position;

    // ��ǥ�� �ʱ�ȭ
    // ī�޶� �� ��ǥ��
    FrontVec = glm::normalize(mainChar->FrontVec);
    LeftVec  = glm::normalize(mainChar->LeftVec);
    UpVec    = glm::normalize(mainChar->UpVec);
    // ī�޶� ��ġ �ʱ�ȭ
    Position = CameraPivot + (-FrontVec) * xzDist + UpVec * yDist;

    // ī�޶� �ٶ󺸴� ����
    Direction = glm::normalize(CameraPivot - Position);
}


/* 
    glm::rotate 
        => ������ ���� �߽����� ȸ���ϴ� ���ʹϾ��� �����ϴ� �� �´µ�
        ���ʹϾ� = ȸ���� ���Ҽ��� ���ؼ� ǥ���� ��
        ������ ������ �࿡ ���� ȸ���� ��Ÿ����

        ���� �̹� ���ó�� Ư�� pivot �� �߽����� ȸ���� �ϴ� ���
        ���������� ȸ������ ������ ����, ȸ���ϰ� �ٽ� ���󺹱��ϴ� ������ �ʿ��ϴ�
 */
// ī�޶��� ��ġ�� ����ϴ� �Լ� => �ǹ��� ��ġ�� ���� ĳ���Ϳ� ���缭 �̵�
void Camera::SetPosition()
{
    // ���� ĳ���Ϳ� �ǹ� ������ ����
    auto PivotToChar = mainChar->Position - CameraPivot;
    
    // �ش� ���� ���̰� ���� ���ϸ� �������� �ʴ´�
    if(!CameraMove && glm::length(PivotToChar) < CamToCharMaxLimit) return;
    

    // �����̱�� ����������
    CameraMove = true;

    // ī�޶� �ӵ��� ���� ī�޶� ���󰣴�
    CameraPivot += PivotToChar * CameraMoveSpeed;
    Position += PivotToChar * CameraMoveSpeed;

    // ���� �� ���Ϸ� ���������, �ٽ� ī�޶� ������Ų��
    PivotToChar = mainChar->Position - CameraPivot;
    if(glm::length(PivotToChar) < CamToCharMinLimit)
        CameraMove = false;
}



// ī�޶� ȸ�� ��Ű�� ���� => �ǹ� �������� ȸ���Ѵ�
void Camera::Rotate(glm::vec2 deltaPos)
{
    auto yawAngle = (-deltaPos.x) * yawRotSpeed;
        if(yawAngle < 0)    yawAngle += 360;
        if(yawAngle > 360)  yawAngle -= 360;

    auto goToOrigin = glm::translate(glm::mat4(1.0f), -(CameraPivot));
    auto yawRotate = glm::rotate(glm::mat4(1.0f), glm::radians(yawAngle), mainChar->UpVec);
    auto comeBack = glm::translate(glm::mat4(1.0f), CameraPivot);

    Position = glm::vec3((comeBack * yawRotate * goToOrigin) * glm::vec4(Position, 1.0f));
    // ī�޶� ��ǥ�赵 ȸ������ �־�� �ϴµ�, �׳� ȸ�� ��ȯ �����ϸ� �ȴ�
    FrontVec = glm::vec3(yawRotate * glm::vec4(FrontVec, 0.0f));
    LeftVec = glm::vec3(yawRotate * glm::vec4(LeftVec, 0.0f));
    Direction = glm::vec3(yawRotate * glm::vec4(Direction, 0.0f));

    // pitchAngle �� ������ �ִ� + ���� �����Ѵ�
    auto pitchAngle = (deltaPos.y) * pitchRotSpeed;
        pitchAngle = glm::clamp(pitchAngle, pitchRotLimit.x, pitchRotLimit.y);

    // Direction ����
    auto pitchRotate = glm::rotate(glm::mat4(1.0f), glm::radians(pitchAngle), LeftVec); // �ٲ� LeftVec �� ���ؼ�
    Direction = glm::vec3(pitchRotate * glm::vec4(Direction, 0.0f));
}