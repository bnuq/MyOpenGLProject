#ifndef __FLOOR_H__
#define __FLOOR_H__

#include "common.h"
#include "mesh.h"


CLASS_PTR(Floor)
class Floor
{
public:
    glm::vec3 Position;

    static MeshPtr m_mesh;

    Floor(glm::vec3 pos) : Position(pos) {}
};

#endif // __FLOOR_H__