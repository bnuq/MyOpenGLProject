#include "Light.h"


void Light::SetPosition(glm::vec3 charPos)
{
    // �׻� ĳ���ͷ� ���� ������ ��ġ�� ������ �ֵ��� �Ѵ�
    Position = charPos + glm::vec3(FarWidth, FarWidth, FarWidth);
}