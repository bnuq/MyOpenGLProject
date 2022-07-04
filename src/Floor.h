#ifndef __FLOOR_H__
#define __FLOOR_H__

#include "common.h"
#include "Box.h"


CLASS_PTR(Floor)
class Floor : public Box
{
public:
    static FloorUPtr Create(glm::vec3 pos, float x = 1.0f, float y = 1.0f, float z = 1.0f);


    glm::vec3 color{};
    bool collision = false;


    bool getTime(float t);
    
private:   
    void setColor();
};

#endif // __FLOOR_H__