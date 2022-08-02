#include "Light.h"


void Light::SetPosition(float charPosY)
{
    // y �� �󿡼� ������ ĳ���Ͱ� ���� ������ �Ÿ�
    float LightCharDistance = Position.y - charPosY;
    
    // ������ �����̰� �־��ٸ�, ���ߴ� ��Ȳ�� �Ǵ� �� Ȯ���Ѵ�
    if(LightMove)
    {
        // ���� �Ÿ� ���ϰ� �ƴٸ� ���� �����
        if(LightCharDistance < yLimitLengthMin)
        {
            LightMove = false;
            // ������ ��ġ�� ��Ȯ�� ���� �ӹ����� �Ѵ�
            Position.y = charPosY + yLimitLengthMin;
        }
        // �ȵƴٸ� ��� �̵��Ѵ�
        else
            Position.y -= LightMoveSpeed;

        return;
    }
    // �� �����̰� �־��ٸ�, �������� �ϴ� ��Ȳ���� Ȯ���Ѵ�
    else
    {
        // ���� �Ÿ� ��������, �������� �ϴ� ��Ȳ
        if(LightCharDistance > yLimitLengthMax)
        {
            LightMove = true;
            Position.y -= LightMoveSpeed;
        }
        
        return;
    }
}