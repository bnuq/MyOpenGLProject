#include "Box.h"
#include <cmath>

bool Box::Collide(BoxPtr a)
{
    auto aPos = a->Position;

    if(abs(aPos.x - Position.x) > (a->xScale * 0.5f + xScale * 0.5f)) return false;
    if(abs(aPos.y - Position.y) > (a->yScale * 0.5f + yScale * 0.5f)) return false;
    if(abs(aPos.z - Position.z) > (a->zScale * 0.5f + zScale * 0.5f)) return false;



    SPDLOG_INFO("Collision happens");
    return true;
}