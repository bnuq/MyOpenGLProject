#ifndef __BOX_H__
#define __BOX_H__

#include "common.h"


CLASS_PTR(Box)
class Box
{
public:
    float xScale;
    float yScale;
    float zScale;

    glm::vec3 Position;

    Box(float x, float y, float z, glm::vec3 pos)
    : xScale(x), yScale(y), zScale(z), Position(pos)
    {}

    bool Collide(const Box* a, const Box* b);
};

#endif // __BOX_H__