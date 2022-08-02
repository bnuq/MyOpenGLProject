#ifndef __LIGHT_H__
#define __LIGHT_H__

#include "../FrameWork/common.h"


CLASS_PTR(Light)
class Light
{
public:

    // ������ ��ġ
    glm::vec3 Position          = glm::vec3(10.0f, 10.0f, 10.0f);

    // ������ �ٶ󺸴� ����
    glm::vec3 Direction         = glm::vec3(-1.0f, -1.0f, -1.0f);

    bool LightMove              = false;
    float yLimitLengthMax       = 15.0f;    // y �� ���� �� ������ �Ÿ��� ���� �̻��̸� �̵��Ѵ�
    float yLimitLengthMin       = 10.0f;    // y �� ���� �� �Ÿ��� �� ���ϰ� �� ������ �̵��Ѵ�
    float LightMoveSpeed        = 0.08f;

    glm::vec3 ambient { glm::vec3(0.4f, 0.4f, 0.4f) };
    glm::vec3 diffuse { glm::vec3(0.8f, 0.8f, 0.8f) };
    glm::vec3 specular { glm::vec3(1.0f, 1.0f, 1.0f) };
    


    // Light Ŭ�� ������ ���� �� orthogonal ������ ����
    // �ش� ������ ���� ��
    float minusX    = -50.0f;
    float plusX     = +50.0f;
    
    float minusY    = -50.0f;
    float plusY     = +50.0f;

    float nearZ     = -5.0f;
    float farZ      = +5.0f;



    // ĳ���Ϳ� ������ �Ÿ��� �ľ�, ������ ��ġ�� �������Ѵ�
    void SetPosition(float charPosY);

};

#endif  // __LIGHT_H__