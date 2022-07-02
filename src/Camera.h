#ifndef __CAM_H__
#define __CAM_H__

#include "common.h"
#include "Character.h"


CLASS_PTR(Camera)
class Camera
{
public:
    // ī�޶� �ٶ󺸴� ������ ȸ���� �߽� �ǹ�
    glm::vec3 CameraPivot;
        float CamToCharMaxLimit = 1.0f;
        float CamToCharMinLimit = 0.1f;

    bool CameraMove = false;
        float CameraMoveSpeed = 0.01f;

        float yawRotSpeed { 0.1f };
        float pitchRotSpeed { 0.1f };
        glm::vec2 pitchRotLimit { glm::vec2{-10.0f, 30.0f} };


    // ī�޶��� ��ġ �ʱ�ȭ
    glm::vec3 Position;

    // ī�޶� �� ��ǥ��
    glm::vec3 FrontVec          = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::vec3 LeftVec           = glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 UpVec             = glm::vec3(0.0f, 1.0f, 0.0f);
    // ī�޶� �ٶ󺸴� ����
    glm::vec3 Direction         = glm::vec3(0.0f, 0.0f, 1.0f);
    

/* ī�޶� ��ġ, �Ÿ� */
    float xzDist = 20.0f;
    float yDist = 10.0f;

    
    // ī�޶� ����ٴϴ� �÷��̾�
    CharacterPtr mainChar;



    static CameraPtr Create(CharacterPtr player);
    void Init(CharacterPtr player);
    

    // ī�޶� ���󰣴� + ���ٴ� ���̸� �ٶ󺻴�
    void SetPosition();
    // ���콺�� �����̴� ��ŭ, ī�޶� ȸ���Ѵ� + �ٶ󺸴� Direction ���͵� �޶�����
    void Rotate(glm::vec2 deltaPos);


private:
    Camera() {}
};

#endif  // __CAM_H__