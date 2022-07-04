#ifndef __BOX_H__
#define __BOX_H__

#include "common.h"
#include "mesh.h"

CLASS_PTR(Box)
class Box
{
public:
    Box() {}
    Box(glm::vec3 pos, float x = 1.0f, float y = 1.0f, float z = 1.0f)
        : xScale(x), yScale(y), zScale(z), Position(pos) {}
    

    float xScale;
    float yScale;
    float zScale;
    glm::vec3 Position;

    bool Collide(BoxPtr a);

private:
    
};

#endif // __BOX_H__