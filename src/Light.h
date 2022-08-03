#ifndef __LIGHT_H__
#define __LIGHT_H__

#include "../FrameWork/common.h"


CLASS_PTR(Light)
class Light
{
public:

    // ������ ��ġ
    glm::vec3 Position;

    // ������ ĳ���ͷκ��� �������� ����� ��ġ�� ��Ÿ���� ����
    float FarWidth    = 5.0f;

    // ������ �ٶ󺸴� ����
    glm::vec3 Direction = glm::normalize(glm::vec3(-FarWidth, -FarWidth, -FarWidth));


    bool LightMove              = false;
    float LightMoveSpeed        = 0.08f;

    glm::vec3 ambient { glm::vec3(0.8f, 0.8f, 0.8f) };
    glm::vec3 diffuse { glm::vec3(0.8f, 0.8f, 0.8f) };
    glm::vec3 specular { glm::vec3(1.0f, 1.0f, 1.0f) };
    


    // Light Ŭ�� ������ ���� �� orthogonal ������ ����
    // �ش� ������ ���� ��
    float minusX = -FarWidth;
    float plusX = FarWidth;
    
    float minusY = -FarWidth;
    float plusY = FarWidth;

    float nearZ = 0.0f;
    float farZ = FarWidth * 10;



    // Constructor
    /* 
        ĳ���ʹ� ���� (0, 0, 0) ���� ����

        ������ �׻� ĳ������ ���� ��ܿ� ��ġ�Ѵٰ� ����
            ĳ���Ͱ� ����ִ���, ĳ���� �������� �׻� ������ ��ġ�� �ִ�
            ĳ���Ͱ� ������ ��, �ٰŸ����� ��� ĳ���͸� ����ٴѴٰ� ����

        ������ directional light �� �����ϱ� ������, ������ ���� ��ġ�� �߿����� �ʴ�
        �ٸ� �׸��ڸ� ����� ���� orthogonal matrix ���Ǹ� ���ؼ� ������ ĳ���͸� ��� ����ٳ�� �Ѵ�
        
        ������ orthogonal matrix �ȿ��� �׻� ĳ���Ͱ� ���� �ȴ�
     */
    Light(glm::vec3 charPos)
    : Position(charPos + glm::vec3(FarWidth, FarWidth, FarWidth))
    {

    }


    // ĳ���Ϳ� ������ �Ÿ��� �ľ�, ������ ��ġ�� �������Ѵ�
    void SetPosition(glm::vec3 charPos);

};

#endif  // __LIGHT_H__