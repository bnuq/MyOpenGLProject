#include "Box.h"
#include <cmath>

// this box �� a ���� ������ ��������, a �� ��´ٰ� ����
bool Box::Collide(BoxPtr floorBox)
{
    auto aPos = floorBox->Position;

    // ���Ϸ��� �ڽ���, �ٴ� �ڽ����� �ؿ� ������ �浹�� �Ͼ �� ����
    if(Position.y < aPos.y) return false;

    if(abs(aPos.x - Position.x) > (floorBox->xScale * 0.5f + xScale * 0.5f)) return false;
    if(abs(aPos.y - Position.y) > (floorBox->yScale * 0.5f + yScale * 0.5f)) return false;
    if(abs(aPos.z - Position.z) > (floorBox->zScale * 0.5f + zScale * 0.5f)) return false;

    return true;
}