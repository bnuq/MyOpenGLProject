#ifndef __CAM_H__
#define __CAM_H__

#include "../FrameWork/common.h"
#include "Character.h"


// ī�޶��� ������ ������ �ִ� ī�޶� ��ü
CLASS_PTR(Camera)
class Camera
{
public:
    // �ǹ� = ī�޶� �ٶ󺸴� ������ ȸ���� �߽�
    glm::vec3 CameraPivot;
        float CamToCharMaxLimit = 0.1f;                         // ī�޶� �ǹ��� �����̰� �ϴ� �Ÿ�
        float CamToCharMinLimit = 0.01f;                        // ī�޶� �ǹ��� ���̻� �������� �ʰ� �ϴ� �Ÿ�
        bool CameraMove = false;                                // ī�޶� �����̰� �ִ� �� ~ ����

    
    // ī�޶� �̵�
    float CameraMoveSpeed = 0.05f;

    
    // ī�޶� ȸ��
    float yawRotSpeed { 0.1f };
    float pitchRotSpeed { 0.1f };
        glm::vec2 pitchRotLimit { glm::vec2{-10.0f, 30.0f} };



    // ī�޶��� ��ġ
    glm::vec3 Position;

    // ī�޶� �� ��ǥ��
    glm::vec3 FrontVec          = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::vec3 LeftVec           = glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 UpVec             = glm::vec3(0.0f, 1.0f, 0.0f);

    // ī�޶� �ٶ󺸴� ����
    glm::vec3 Direction         = glm::vec3(0.0f, 0.0f, 1.0f);
    


    // ī�޶� ����ٴϴ� ĳ����
    CharacterPtr mainChar;
        // ī�޶� ĳ���ͷ� ���� ������ �Ÿ� 
        float xzDist = 20.0f;
        float yDist = 20.0f;


    // ���� �� �ʱ�ȭ
    static CameraPtr Create(CharacterPtr player);
    void Init(CharacterPtr player);
    

    // ������ ĳ���Ϳ� ����, ī�޶� �̵���Ŵ
    void SetPosition();


    // ���콺�� �����̴� ��ŭ, ī�޶� ȸ���Ѵ� + �ٶ󺸴� Direction ���͵� �޶�����
    void Rotate(glm::vec2 deltaPos);


private:
    Camera() {}
};

#endif  // __CAM_H__