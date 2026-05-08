//
// Created by nicolas on 12.03.26.
//

#include "Triangle.h"
#include "../util/AABB.h"
#include <cmath>

Triangle::Triangle(const Vector3d &a, const Vector3d &b, const Vector3d &c, const Material &material) : Object(material), a(a), b(b), c(c) {
    cross = (b-a)/(c-a);
    normal = cross/cross.getLength();
}

Triangle::Triangle(const Vector3d &a, const Vector3d &b, const Vector3d &c) : Object(Material(Color(0, 0, 0))), a(a), b(b), c(c) {
    cross = (b-a)/(c-a);
    normal = cross/cross.getLength();
}

Vector3d Triangle::getA() const {
    return a;
}

Vector3d Triangle::getB() const {
    return b;
}

Vector3d Triangle::getC() const {
    return c;
}

Vector3d Triangle::getNormal() const {
    return normal;
}

double Triangle::getArea() const {
    return 0.5*cross.getLength();
}

AABB Triangle::getBoundingBox() const {
    AABB box;
    box.expand(a);
    box.expand(b);
    box.expand(c);
    return box;
}

Hit Triangle::intersect(Ray ray) const {
    const Vector3d edge1 = b - a;
    const Vector3d edge2 = c - a;

    // h = rayDir × edge2
    const Vector3d h = ray.getDirection() / edge2;
    const double det = edge1 * h;

    // Ray parallel to triangle plane
    if (std::abs(det) < 1e-10)
        return Hit(-1, Vector3d(0, 0, 0), normal, getMaterial());

    const double invDet = 1.0 / det;
    const Vector3d s = ray.getLocation() - a;
    const double u = invDet * (s * h);

    if (u < 0.0 || u > 1.0)
        return Hit(-1, Vector3d(0, 0, 0), normal, getMaterial());

    // q = s × edge1
    const Vector3d q = s / edge1;
    const double v = invDet * (ray.getDirection() * q);

    if (v < 0.0 || u + v > 1.0)
        return Hit(-1, Vector3d(0, 0, 0), normal, getMaterial());

    const double lambda = invDet * (edge2 * q);

    if (lambda < 1e-10)
        return Hit(-1, Vector3d(0, 0, 0), normal, getMaterial());

    Hit hit(lambda, ray.getLocation() + ray.getDirection() * lambda, normal, getMaterial());
    hit.setFrontFace(ray.getDirection(), normal);
    hit.setU(u);
    hit.setV(v);
    return hit;
}
