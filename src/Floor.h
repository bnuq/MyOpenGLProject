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
    double SavedTime;
    const double LimitTime = 5.0;
    bool Disappear = false;

    void SetTime(double t)
    {
        SavedTime = t;
        collision = true;
    }
    bool CheckTime(double t);
    
private:   
    void setColor();
};

#endif // __FLOOR_H__