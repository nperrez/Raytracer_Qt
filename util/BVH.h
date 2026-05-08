//
// Created by nicolas on 08.05.26.
//

#ifndef RAYTRACER_QT_BVH_H
#define RAYTRACER_QT_BVH_H

#include <vector>
#include <utility>
#include "AABB.h"
#include "Hit.h"
#include "Ray.h"

class Object;

class BVH {
    struct Node {
        AABB    box;
        int     left  = -1;    // index into nodes; -1 means this is a leaf
        int     right = -1;
        Object* leaf  = nullptr;
    };

    std::vector<Node> nodes;

    int buildRecursive(std::vector<std::pair<Object*, AABB>>& items, int start, int end);
    Hit traverseNode(int idx, Ray ray, double tMin, double tMax) const;

public:
    BVH() = default;
    void build(std::vector<Object*> objects);
    Hit  traverse(Ray ray) const;
    bool empty() const { return nodes.empty(); }
};

#endif //RAYTRACER_QT_BVH_H
