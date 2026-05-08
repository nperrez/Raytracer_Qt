//
// Created by nicolas on 08.05.26.
//

#include "BVH.h"
#include "../3dObjects/Object.h"
#include <algorithm>
#include <limits>

// ---------------------------------------------------------------------------
// Build
// ---------------------------------------------------------------------------

int BVH::buildRecursive(std::vector<std::pair<Object*, AABB>>& items, int start, int end) {
    int idx = (int)nodes.size();
    nodes.emplace_back();

    // Compute the combined bounding box for everything in [start, end)
    AABB box;
    for (int i = start; i < end; i++)
        box.expand(items[i].second);
    nodes[idx].box = box;

    int count = end - start;

    // Leaf: only one object left
    if (count == 1) {
        nodes[idx].leaf = items[start].first;
        return idx;
    }

    // Find the axis along which the object centroids are most spread out.
    // Splitting on the widest axis gives the most balanced tree.
    AABB centroidBox;
    for (int i = start; i < end; i++) {
        // Centroid = midpoint of object's bounding box
        Vector3d c = (items[i].second.min + items[i].second.max) * 0.5;
        centroidBox.expand(c);
    }
    Vector3d extent = centroidBox.max - centroidBox.min;
    int axis = 0;
    if (extent.getY() > extent.getX()) axis = 1;
    if (extent.getZ() > (axis == 0 ? extent.getX() : extent.getY())) axis = 2;

    // Partition: put the median object at 'mid' so that everything in
    // [start, mid) has a smaller centroid than everything in [mid, end)
    // along the chosen axis.  nth_element does this in O(n).
    int mid = (start + end) / 2;
    std::nth_element(items.begin() + start, items.begin() + mid, items.begin() + end,
        [axis](const std::pair<Object*, AABB>& a, const std::pair<Object*, AABB>& b) {
            double ca, cb;
            if      (axis == 0) { ca = a.second.min.getX() + a.second.max.getX();
                                  cb = b.second.min.getX() + b.second.max.getX(); }
            else if (axis == 1) { ca = a.second.min.getY() + a.second.max.getY();
                                  cb = b.second.min.getY() + b.second.max.getY(); }
            else                { ca = a.second.min.getZ() + a.second.max.getZ();
                                  cb = b.second.min.getZ() + b.second.max.getZ(); }
            return ca < cb;
        });

    nodes[idx].left  = buildRecursive(items, start, mid);
    nodes[idx].right = buildRecursive(items, mid,   end);
    return idx;
}

void BVH::build(std::vector<Object*> objects) {
    if (objects.empty()) return;
    nodes.clear();
    // A binary tree with N leaves has at most 2N-1 nodes total.
    // Reserving upfront prevents reallocation during recursion,
    // which would invalidate the node references we write into via nodes[idx].
    nodes.reserve(2 * objects.size());

    // Pre-compute each object's bounding box once here so
    // buildRecursive doesn't recompute it on every comparison.
    std::vector<std::pair<Object*, AABB>> items;
    items.reserve(objects.size());
    for (Object* obj : objects)
        items.emplace_back(obj, obj->getBoundingBox());

    buildRecursive(items, 0, (int)items.size());
}

// ---------------------------------------------------------------------------
// Traverse
// ---------------------------------------------------------------------------

Hit BVH::traverseNode(int idx, Ray ray, double tMin, double tMax) const {
    const Node& node = nodes[idx];

    // Box miss: the ray doesn't even enter this node's volume, skip the subtree
    if (!node.box.hit(ray, tMin, tMax))
        return Hit();

    // Leaf: test the actual object
    if (node.leaf)
        return node.leaf->intersect(ray);

    // Internal node: test both children.
    // After testing the left child, if it returned a hit, use its distance
    // as the new tMax when testing the right child.  This means the right
    // child's box test will reject any part of the tree that can't possibly
    // be closer than the left hit, pruning entire subtrees for free.
    Hit  leftHit  = traverseNode(node.left,  ray, tMin, tMax);
    double newTMax = (leftHit.getLambda() > 0) ? leftHit.getLambda() : tMax;
    Hit  rightHit = traverseNode(node.right, ray, tMin, newTMax);

    if (rightHit.getLambda() > 0) return rightHit;
    return leftHit;
}

Hit BVH::traverse(Ray ray) const {
    if (nodes.empty()) return Hit();
    return traverseNode(0, ray, 0.0, std::numeric_limits<double>::infinity());
}
