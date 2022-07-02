#include "Box.h"
#include <cmath>

// this box 가 a 보다 위에서 내려간다, a 를 밟는다고 생각
bool Box::Collide(BoxPtr floorBox)
{
    auto aPos = floorBox->Position;

    // 비교하려는 박스가, 바닥 박스보다 밑에 있으면 충돌이 일어날 수 없다
    if(Position.y < aPos.y) return false;

    if(abs(aPos.x - Position.x) > (floorBox->xScale * 0.5f + xScale * 0.5f)) return false;
    if(abs(aPos.y - Position.y) > (floorBox->yScale * 0.5f + yScale * 0.5f)) return false;
    if(abs(aPos.z - Position.z) > (floorBox->zScale * 0.5f + zScale * 0.5f)) return false;

    return true;
}