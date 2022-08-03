#include "Light.h"


void Light::SetPosition(glm::vec3 charPos)
{
    // 항상 캐릭터로 부터 동일한 위치에 광원이 있도록 한다
    Position = charPos + glm::vec3(FarWidth, FarWidth, FarWidth);
}