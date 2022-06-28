#ifndef __BOX_H__
#define __BOX_H__

#include "common.h"


class Box
{
public:
    Box(float x, float y, float z, glm::vec3 pos)
        : xScale(x), yScale(y), zScale(z), Position(pos) {}
    
    float xScale;
    float yScale;
    float zScale;

    glm::vec3 Position;

    

    bool Collide(const Box& a);
};

#endif // __BOX_H__