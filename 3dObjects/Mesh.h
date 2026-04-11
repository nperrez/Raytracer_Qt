//
// Created by nicolas on 09.04.26.
//

#ifndef RAYTRACER_QT_MESH_H
#define RAYTRACER_QT_MESH_H
#include <string>
#include <unordered_map>
#include <vector>

#include "Object.h"
#include "Triangle.h"


class Mesh : public Object {
private:
    std::vector<Triangle> triangles;

    static std::unordered_map<std::string, Material> parseMtl(const std::string &mtlPath);

public:
    Mesh();
    explicit Mesh(std::vector<Triangle> triangles);
    static Mesh fromObj(const std::string &objPath);
    Hit intersect(Ray ray) const override;

};


#endif //RAYTRACER_QT_MESH_H