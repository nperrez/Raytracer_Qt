//
// Created by nicolas on 09.04.26.
//

#ifndef RAYTRACER_QT_MESH_H
#define RAYTRACER_QT_MESH_H
#include <vector>

#include "Object.h"
#include "Triangle.h"


class Mesh : public Object {
private:
    std::vector<Triangle> triangles;

public:
    Mesh();
    Hit intersect(Ray ray) const override;

};


#endif //RAYTRACER_QT_MESH_H