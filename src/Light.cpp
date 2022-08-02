#include "Light.h"


void Light::SetPosition(float charPosY)
{
    // y 축 상에서 광원과 캐릭터간 서로 떨어진 거리
    float LightCharDistance = Position.y - charPosY;
    
    // 광원이 움직이고 있었다면, 멈추는 상황이 되는 지 확인한다
    if(LightMove)
    {
        // 일정 거리 이하가 됐다면 이제 멈춘다
        if(LightCharDistance < yLimitLengthMin)
        {
            LightMove = false;
            // 광원의 위치를 정확한 곳에 머무르게 한다
            Position.y = charPosY + yLimitLengthMin;
        }
        // 안됐다면 계속 이동한다
        else
            Position.y -= LightMoveSpeed;

        return;
    }
    // 안 움직이고 있었다면, 움직여야 하는 상황인지 확인한다
    else
    {
        // 일정 거리 떨어졌어, 움직여야 하는 상황
        if(LightCharDistance > yLimitLengthMax)
        {
            LightMove = true;
            Position.y -= LightMoveSpeed;
        }
        
        return;
    }
}