#include <ctime>
#include <cstdlib>

#include "Floor.h"


FloorUPtr Floor::Create(glm::vec3 pos, float x, float y, float z)
{
    auto fl = FloorUPtr(new Floor{});
    fl->Position = pos;

    fl->xScale = x;
    fl->yScale = y;
    fl->zScale = z;

    return std::move(fl);
}




void Floor::setColor()
{
    std::srand(static_cast<unsigned int>(std::time(0)));

    color.x = static_cast<float>(std::rand()) / (RAND_MAX + 1);
    color.y = static_cast<float>(std::rand()) / (RAND_MAX + 1);
    color.z = static_cast<float>(std::rand()) / (RAND_MAX + 1);
}



bool Floor::CheckTime(double d)
{
    if(d - SavedTime > LimitTime) return true;
    else return false;
}