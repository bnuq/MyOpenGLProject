#include <ctime>
#include <cstdlib>

#include "Floor.h"


void Floor::setColor()
{
    std::srand(static_cast<unsigned int>(std::time(0)));

    color.x = static_cast<float>(std::rand()) / (RAND_MAX + 1);
    color.y = static_cast<float>(std::rand()) / (RAND_MAX + 1);
    color.z = static_cast<float>(std::rand()) / (RAND_MAX + 1);
}