#ifndef __FLOOR_H__
#define __FLOOR_H__

#include "common.h"
#include "Box.h"


CLASS_PTR(Floor)
class Floor : public Box
{
public:
    glm::vec3 color{};
    bool collision;

    Floor(glm::vec3 pos, float x = 1.0f, float y = 1.0f, float z = 1.0f)
    : Box(pos, x, y, z)
    {
        collision = false;
        // ������ ������ ������ �Ѵ�
        setColor();
    }

    bool getTime(float t);
    
private:   
    void setColor();
};

#endif // __FLOOR_H__