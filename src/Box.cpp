#include "Box.h"
#include <cmath>

bool Box::Collide(const Box* a, const Box* b)
{
    auto aPos = a->Position;
    auto bPos = b->Position;

    if(abs(aPos.x - bPos.x) > (a->xScale * 0.5f + b->xScale * 0.5f)) return false;
    if(abs(aPos.y - bPos.y) > (a->yScale * 0.5f + b->yScale * 0.5f)) return false;
    if(abs(aPos.z - bPos.z) > (a->zScale * 0.5f + b->zScale * 0.5f)) return false;

    return true;
}